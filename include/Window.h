#pragma once

#include <Windows.h>
#include "ConsoleBuffer.h"
#include <vector>
#include <algorithm>
#include <assert.h>

class Window
{
    public:
		Window();
        virtual ~Window();

        virtual void OnWindowRefreshed();
		virtual void OnWindowResized(int width, int height);
        virtual void OnKeyEvent(const KEY_EVENT_RECORD& ker);

        static void SetConsoleBuffer(ConsoleBuffer* buffer);
        static void Add(Window* window);
        static void Remove(Window* window);
        static void Resize(int width, int height);
        static void Refresh(bool fullDraw);
        static void KeyEvent(const KEY_EVENT_RECORD& ker);

    protected:
        int m_width;
        int m_height;

        static ConsoleBuffer* s_consoleBuffer;

    private:
        static std::vector<Window*> s_windows;
};

inline void Window::SetConsoleBuffer(ConsoleBuffer* buffer)
{
    s_consoleBuffer = buffer;
}

inline void Window::Add(Window* window)
{
    s_windows.push_back(window);
}

inline void Window::Remove(Window* window)
{
    auto iter = std::remove(s_windows.begin(), s_windows.end(), window);
    s_windows.erase(iter);
}

inline void Window::Resize(int width, int height)
{
    s_consoleBuffer->OnWindowResize(width, height);
    for (size_t i = 0; i < s_windows.size(); i++)
        s_windows[i]->OnWindowResized(width, height);
}

inline void Window::Refresh(bool fullDraw)
{
    for (size_t i = 0; i < s_windows.size(); i++)
        s_windows[i]->OnWindowRefreshed();
    assert(s_consoleBuffer);
    s_consoleBuffer->Flush(fullDraw);
}

inline void Window::KeyEvent(const KEY_EVENT_RECORD& ker)
{
    for (size_t i = 0; i < s_windows.size(); i++)
        s_windows[i]->OnKeyEvent(ker);
}

inline void Window::OnKeyEvent(const KEY_EVENT_RECORD& ker)
{
}

inline void Window::OnWindowRefreshed()
{
}
