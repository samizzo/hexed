#pragma once

#include <Windows.h>

class ConsoleBuffer
{
    public:
        ConsoleBuffer(HANDLE stdoutHandle);
        ~ConsoleBuffer();

        bool IsInitialised() const;
        void Write(int x, int y, WORD attributes, const char* format, ...);
        void FillLine(int y, char c, WORD attributes);
        void FillRect(int x, int y, int width, int height, char c, WORD attributes);
        void Clear();
        void OnWindowResize(int width, int height);
        void Flush(bool fullDraw);

        int GetWidth() const;
        int GetHeight() const;

    private:
        int m_width;
        int m_height;
        CHAR_INFO* m_buffer;
        CHAR_INFO* m_backBuffer;
        HANDLE m_stdoutHandle;
};

inline bool ConsoleBuffer::IsInitialised() const
{
    return m_buffer != 0;
}

inline int ConsoleBuffer::GetWidth() const
{
    return m_width;
}

inline int ConsoleBuffer::GetHeight() const
{
    return m_height;
}
