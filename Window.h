#pragma once

#include <Windows.h>

class Window
{
    public:
		Window();
        ~Window();

        virtual void Draw();

		void Write(int x, int y, const char* text, WORD attributes);
		void FillLine(int y, char c, WORD attributes);
        void Flush(HANDLE handle);

		virtual void OnWindowResize(int newWidth, int newHeight);

        void SetPosition(int x, int y);

    protected:
        int m_width;
        int m_height;
        int m_x;
        int m_y;

        CHAR_INFO* m_buffer;
};

inline void Window::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}
