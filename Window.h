#pragma once

#include <Windows.h>
#include "ConsoleBuffer.h"

class Window
{
    public:
		Window(ConsoleBuffer* consoleBuffer);

        virtual void Draw();
		virtual void OnWindowResize(int width, int height);

        void SetPosition(int x, int y);

    protected:
        int m_width;
        int m_height;
        int m_x;
        int m_y;
        ConsoleBuffer* m_consoleBuffer;
};

inline void Window::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}
