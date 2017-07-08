#pragma once

#include "ConsoleBuffer.h"
#include "KeyEvent.h"
#include <vector>
#include <assert.h>

class Window
{
    public:
		Window();
        Window(Window* parent);

        virtual void OnWindowRefreshed();
		virtual void OnWindowResized(int width, int height);
        virtual void OnKeyEvent(KeyEvent& keyEvent);

        void AddChild(Window* window);

        void SetVisible(bool visible);
        bool IsVisible() const;

        static void SetConsoleBuffer(ConsoleBuffer* buffer);
        static void Add(Window* window);
        static void Resize(int width, int height);
        static void Refresh(bool fullDraw);
        static void ProcessKeyInput(KeyEvent& keyEvent);

    protected:
        int m_width;
        int m_height;
        Window* m_parent;

        static ConsoleBuffer* s_consoleBuffer;

    private:
        std::vector<Window*> m_children;
        bool m_visible;

        static std::vector<Window*> s_rootWindows;
};

inline Window::Window()
{
    m_width = 0;
    m_height = 0;
    m_visible = true;
    m_parent = 0;
    Add(this);
}

inline Window::Window(Window* parent)
{
    m_width = 0;
    m_height = 0;
    parent->AddChild(this);
}

inline void Window::SetConsoleBuffer(ConsoleBuffer* buffer)
{
    s_consoleBuffer = buffer;
}

inline void Window::Add(Window* window)
{
    s_rootWindows.push_back(window);
}

inline void Window::Resize(int width, int height)
{
    s_consoleBuffer->OnWindowResize(width, height);
    for (size_t i = 0; i < s_rootWindows.size(); i++)
    {
        Window* window = s_rootWindows[i];
        if (window->IsVisible())
            window->OnWindowResized(width, height);
    }
}

inline void Window::Refresh(bool fullDraw)
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

inline void Window::ProcessKeyInput(KeyEvent& keyEvent)
{
    for (size_t i = 0; i < s_rootWindows.size(); i++)
    {
        Window* window = s_rootWindows[i];
        if (window->IsVisible())
            window->OnKeyEvent(keyEvent);
    }
}

inline void Window::OnKeyEvent(KeyEvent& keyEvent)
{
    for (size_t i = 0; i < m_children.size(); i++)
    {
        Window* window = m_children[i];
        if (window->IsVisible())
            window->OnKeyEvent(keyEvent);
    }
}

inline void Window::OnWindowRefreshed()
{
    for (size_t i = 0; i < m_children.size(); i++)
    {
        Window* window = m_children[i];
        if (window->IsVisible())
            window->OnWindowRefreshed();
    }
}

inline void Window::OnWindowResized(int width, int height)
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

inline void Window::AddChild(Window* window)
{
    m_children.push_back(window);
    window->m_parent = this;
}

inline void Window::SetVisible(bool visible)
{
    m_visible = visible;
    if (m_parent && m_visible)
        OnWindowResized(m_parent->m_width, m_parent->m_height);
    Window::Refresh(true);
}

inline bool Window::IsVisible() const
{
    return m_visible;
}
