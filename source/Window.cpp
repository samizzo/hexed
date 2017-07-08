#include "Window.h"

std::vector<Window*> Window::s_visibleWindows;
ConsoleBuffer* Window::s_consoleBuffer;

Window::Window()
{
    m_width = 0;
    m_height = 0;
    m_flags = 0;
    SetVisible(true);
}

void Window::SetConsoleBuffer(ConsoleBuffer* buffer)
{
    s_consoleBuffer = buffer;
}

void Window::Add(Window* window)
{
    s_visibleWindows.push_back(window);
}

void Window::Resize(int width, int height)
{
    s_consoleBuffer->OnWindowResize(width, height);
    for (size_t i = 0; i < s_visibleWindows.size(); i++)
    {
        Window* window = s_visibleWindows[i];
        if (window->IsVisible())
            window->OnWindowResized(width, height);
    }
}

void Window::Refresh(bool fullDraw)
{
    if (!s_consoleBuffer || !s_consoleBuffer->IsInitialised())
        return;

    for (size_t i = 0; i < s_visibleWindows.size(); i++)
    {
        Window* window = s_visibleWindows[i];
        if (window->IsVisible())
            window->OnWindowRefreshed();
    }
    assert(s_consoleBuffer);
    s_consoleBuffer->Flush(fullDraw);
}

void Window::ProcessKeyInput(KeyEvent& keyEvent)
{
    // Input is processed in visiblity order.
    for (int i = (int)s_visibleWindows.size() - 1; i >= 0; i--)
    {
        Window* window = s_visibleWindows[i];
        window->OnKeyEvent(keyEvent);

        // If a window handled the event, no other windows will handled it.
        if (keyEvent.WasHandled())
            break;
    }
}

void Window::OnWindowResized(int width, int height)
{
    m_width = width;
    m_height = height;
}

void Window::SetVisible(bool visible)
{
    m_flags = visible ? m_flags | Flags_Visible : m_flags & ~Flags_Visible;
    if (IsVisible())
    {
        if (s_consoleBuffer && s_consoleBuffer->IsInitialised())
            OnWindowResized(s_consoleBuffer->GetWidth(), s_consoleBuffer->GetHeight());

        // Remove from the visible list first.
        for (size_t i = 0; i < s_visibleWindows.size(); i++)
        {
            if (s_visibleWindows[i] == this)
            {
                s_visibleWindows.erase(s_visibleWindows.begin() + i);
                break;
            }
        }

        // Now add to the end.
        s_visibleWindows.push_back(this);
    }
    else
    {
        // Remove from the visible list.
        for (size_t i = 0; i < s_visibleWindows.size(); i++)
        {
            if (s_visibleWindows[i] == this)
            {
                s_visibleWindows.erase(s_visibleWindows.begin() + i);
                break;
            }
        }
    }

    Window::Refresh(true);
}

bool Window::IsVisible() const
{
    return (m_flags & Flags_Visible) != 0;
}
