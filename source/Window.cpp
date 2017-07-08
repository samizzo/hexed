#include "Window.h"

std::vector<Window*> Window::s_rootWindows;
std::vector<Window*> Window::s_focusWindows;
ConsoleBuffer* Window::s_consoleBuffer;

Window::Window()
{
    m_width = 0;
    m_height = 0;
    m_flags = Flags_Visible;
    Add(this);
}

void Window::SetConsoleBuffer(ConsoleBuffer* buffer)
{
    s_consoleBuffer = buffer;
}

void Window::Add(Window* window)
{
    s_rootWindows.push_back(window);
}

void Window::Resize(int width, int height)
{
    s_consoleBuffer->OnWindowResize(width, height);
    for (size_t i = 0; i < s_rootWindows.size(); i++)
    {
        Window* window = s_rootWindows[i];
        if (window->IsVisible())
            window->OnWindowResized(width, height);
    }
}

void Window::Refresh(bool fullDraw)
{
    if (!s_consoleBuffer->IsInitialised())
        return;

    for (size_t i = 0; i < s_rootWindows.size(); i++)
    {
        Window* window = s_rootWindows[i];
        if (window->IsVisible())
            window->OnWindowRefreshed();
    }
    assert(s_consoleBuffer);
    s_consoleBuffer->Flush(fullDraw);
}

void Window::ProcessKeyInput(KeyEvent& keyEvent)
{
    // Input is processed in order of focussed window.
    for (int i = (int)s_focusWindows.size() - 1; i >= 0; i--)
    {
        Window* window = s_focusWindows[i];
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
        OnWindowResized(s_consoleBuffer->GetWidth(), s_consoleBuffer->GetHeight());

    SetFocus(IsVisible());
    Window::Refresh(true);
}

bool Window::IsVisible() const
{
    return (m_flags & Flags_Visible) != 0;
}

void Window::SetFocus(bool focus)
{
    if (focus)
    {
        // We remove focus first in case the window was already in the focus list.
        // This way it will be forced to have focus and be topmost.
        SetFocus(false);
        s_focusWindows.push_back(this);
    }
    else
    {
        for (size_t i = 0; i < s_focusWindows.size(); i++)
        {
            if (s_focusWindows[i] == this)
            {
                s_focusWindows.erase(s_focusWindows.begin() + i);
                break;
            }
        }
    }
}
