#pragma once

#include <Windows.h>
#include "ConsoleBuffer.h"
#include <vector>
#include <algorithm>

class Window
{
    public:
		Window(ConsoleBuffer* consoleBuffer, const char* filename);
        ~Window();

        virtual void OnWindowRefreshed();
		virtual void OnWindowResized(int width, int height);

        static void Add(Window* window);
        static void Remove(Window* window);
        static void Resize(int width, int height);
        static void Refresh();

    protected:
        const char* m_filename;
        int m_width;
        int m_height;
        ConsoleBuffer* m_consoleBuffer;

    private:
        static std::vector<Window*> s_windows;
};

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
    for (size_t i = 0; i < s_windows.size(); i++)
        s_windows[i]->OnWindowResized(width, height);
}

inline void Window::Refresh()
{
    for (size_t i = 0; i < s_windows.size(); i++)
        s_windows[i]->OnWindowRefreshed();
}
