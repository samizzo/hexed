#include "ConsoleBuffer.h"
#include "Colours.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

ConsoleBuffer::ConsoleBuffer(HANDLE stdoutHandle)
{
    m_stdoutHandle = stdoutHandle;
    m_width = 0;
    m_height = 0;
    m_buffer = 0;
    m_backBuffer = 0;
}

ConsoleBuffer::~ConsoleBuffer()
{
    delete[] m_buffer;
    delete[] m_backBuffer;
}

void ConsoleBuffer::Write(int x, int y, WORD attributes, const char* format, ...)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    va_list args;
    va_start(args, format);
    int count = _vscprintf(format, args);
    char* buffer = (char*)_alloca(count + 1);
    vsprintf_s(buffer, count + 1, format, args);

    int offset = x + (y * m_width);
    for (int i = 0; i < count; i++, offset++)
    {
        m_buffer[offset].Char.AsciiChar = buffer[i];
        m_buffer[offset].Attributes = attributes;
    }

    va_end(args);
}

void ConsoleBuffer::SetAttributes(int x, int y, WORD attributes)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    int offset = x + (y * m_width);
    m_buffer[offset].Attributes = attributes;
}

void ConsoleBuffer::DrawWindow(int x, int y, int width, int height, WORD colour)
{
    WORD shadowColour = Colours::Shadow;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char c = ' ';

            // Horizontal border.
            if (j == 0 || j == height - 1)
                c = 205;

            Write(i + x, j + y, colour, "%c", c);

            if (j == 0)
            {
                // Bottom shadow.
                SetAttributes(i + x + 2, y + height, shadowColour);
            }
        }

        // Vertical border.
        if (j > 0 && j < height - 1)
        {
            Write(x, j + y, colour, "%c", 186);
            Write(x + width - 1, j + y, colour, "%c", 186);
        }

        // Right shadow.
        SetAttributes(x + width, j + y + 1, shadowColour);
        SetAttributes(x + width + 1, j + y + 1, shadowColour);
    }

    // Corners of box.
    Write(x, y, colour, "%c", 201);
    Write(x + width - 1, y, colour, "%c", 187);
    Write(x, y + height - 1, colour, "%c", 200);
    Write(x + width - 1, y + height - 1, colour, "%c", 188);
}

void ConsoleBuffer::FillLine(int y, char c, WORD attributes)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    int offset = y * m_width;
    for (int i = 0; i < m_width; i++, offset++)
    {
        m_buffer[offset].Char.AsciiChar = c;
        m_buffer[offset].Attributes = attributes;
    }
}

void ConsoleBuffer::FillRect(int x, int y, int width, int height, char c, WORD attributes)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    for (int j = y; j < y + height; j++)
    {
        for (int i = x; i < x + width; i++)
        {
            CHAR_INFO& charInfo = m_buffer[i + (j * m_width)];
            charInfo.Char.AsciiChar = c;
            charInfo.Attributes = attributes;
        }
    }
}

void ConsoleBuffer::Clear(WORD clearColour)
{
    assert(m_buffer != 0);
    assert(m_width > 0);
    assert(m_height > 0);

    for (int i = 0; i < m_width * m_height; i++)
    {
        CHAR_INFO& info = m_buffer[i];
        info.Char.AsciiChar = ' ';
        info.Attributes = clearColour;
    }
}

void ConsoleBuffer::OnWindowResize(int width, int height)
{
    delete[] m_buffer;
    m_width = width;
    m_height = height;
    m_buffer = new CHAR_INFO[width * height];

    delete[] m_backBuffer;
    m_width = width;
    m_height = height;
    m_backBuffer = new CHAR_INFO[width * height];
}

void ConsoleBuffer::Flush(bool fullDraw)
{
    assert(m_stdoutHandle != 0);
    assert(m_buffer != 0);
    assert(m_backBuffer);
    assert(m_width > 0);
    assert(m_height > 0);

    COORD bufferSize;
    bufferSize.X = m_width;
    bufferSize.Y = m_height;

    COORD bufferCoord;
    bufferCoord.X = 0;
    bufferCoord.Y = 0;

    SMALL_RECT rect;
    rect.Top = 0;
    rect.Left = 0;
    rect.Bottom = m_height - 1;
    rect.Right = m_width - 1;

    if (fullDraw)
    {
        memcpy(m_backBuffer, m_buffer, m_width * m_height * sizeof(CHAR_INFO));
        WriteConsoleOutput(m_stdoutHandle, m_buffer, bufferSize, bufferCoord, &rect);
    }
    else
    {
        int index = 0;
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++, index++)
            {
                CHAR_INFO& c = m_buffer[index];
                CHAR_INFO& bc = m_backBuffer[index];
                COORD coord;
                coord.X = x;
                coord.Y = y;

                //if (c.Attributes != bc.Attributes || c.Char.AsciiChar != bc.Char.AsciiChar)
                //{
                //    bufferSize.X = 1;
                //    bufferSize.Y = 1;
                //    coord.X = 0;
                //    coord.Y = 0;
                //    rect.Top = y;
                //    rect.Left = x;
                //    rect.Bottom = y;
                //    rect.Right = x;
                //    bc.Attributes = c.Attributes;
                //    bc.Char.AsciiChar = c.Char.AsciiChar;
                //    WriteConsoleOutput(m_stdoutHandle, &c, bufferSize, coord, &rect);
                //}

                if (c.Attributes != bc.Attributes)
                {
                    bc.Attributes = c.Attributes;
                    DWORD numWritten;
                    WriteConsoleOutputAttribute(m_stdoutHandle, &bc.Attributes, 1, coord, &numWritten);
                }

                if (c.Char.AsciiChar != bc.Char.AsciiChar)
                {
                    bc.Char.AsciiChar = c.Char.AsciiChar;
                    DWORD numWritten;
                    WriteConsoleOutputCharacter(m_stdoutHandle, &bc.Char.AsciiChar, 1, coord, &numWritten);
                }
            }
        }
    }
}

void ConsoleBuffer::SetCursor(bool visible, unsigned int size)
{
    CONSOLE_CURSOR_INFO info;
    info.bVisible = visible;
    info.dwSize = size;
    SetConsoleCursorInfo(m_stdoutHandle, &info);
}
