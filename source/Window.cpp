#include "Window.h"

std::vector<Window*> Window::s_rootWindows;
ConsoleBuffer* Window::s_consoleBuffer;

Window::Window()
{
    m_width = 0;
    m_height = 0;
    m_flags = Flags_Visible | Flags_Focus;
    m_parent = 0;
    Add(this);
}

Window::Window(Window* parent)
{
    m_flags = Flags_Visible | Flags_Focus;
    m_width = 0;
    m_height = 0;
    parent->AddChild(this);
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
    for (size_t i = 0; i < s_rootWindows.size(); i++)
    {
        Window* window = s_rootWindows[i];
        if (window->IsVisible() && window->HasFocus())
            window->OnKeyEvent(keyEvent);
    }
}

void Window::OnKeyEvent(KeyEvent& keyEvent)
{
    for (size_t i = 0; i < m_children.size(); i++)
    {
        Window* window = m_children[i];
        if (window->IsVisible() && window->HasFocus())
            window->OnKeyEvent(keyEvent);
    }
}

void Window::OnWindowRefreshed()
{
    for (size_t i = 0; i < m_children.size(); i++)
    {
        Window* window = m_children[i];
        if (window->IsVisible())
            window->OnWindowRefreshed();
    }
}

void Window::OnWindowResized(int width, int height)
{
    m_width = width;
    m_height = height;
    for (size_t i = 0; i < m_children.size(); i++)
    {
        Window* window = m_children[i];
        if (window->IsVisible())
            window->OnWindowResized(width, height);
    }
}

void Window::AddChild(Window* window)
{
    m_children.push_back(window);
    window->m_parent = this;
}

void Window::SetVisible(bool visible)
{
    m_flags = visible ? m_flags | Flags_Visible : m_flags & ~Flags_Visible;
    if (m_parent && IsVisible())
        OnWindowResized(m_parent->m_width, m_parent->m_height);
    Window::Refresh(true);
}

bool Window::IsVisible() const
{
    return (m_flags & Flags_Visible) != 0;
}

void Window::SetFocus(bool focus)
{
    m_flags = focus ? m_flags | Flags_Focus : m_flags & ~Flags_Focus;
}

bool Window::HasFocus() const
{
    return (m_flags & Flags_Focus) != 0;
}
