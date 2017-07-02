#include "Window.h"
#include <assert.h>

Window::Window()
{
	m_width = 0;
	m_height = 0;
	m_buffer = 0;
    m_x = 0;
    m_y = 0;
}

Window::~Window()
{
	delete[] m_buffer;
}

void Window::Draw()
{
	assert(m_buffer != 0);
	assert(m_width > 0);
	assert(m_height > 0);

    // Fill background.
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_buffer[i].Attributes = FOREGROUND_RED;
        m_buffer[i].Char.AsciiChar = ' ';
    }

	FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
	Write(2, 0, "hexed.exe", BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
	FillLine(m_height - 1, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
}

void Window::Write(int x, int y, const char* text, WORD attributes)
{
	assert(m_buffer != 0);
	assert(m_width > 0);
	assert(m_height > 0);

	int offset = x + (y * m_width);
	int length = strlen(text);
	for (int i = 0; i < length; i++, offset++)
	{
		m_buffer[offset].Char.AsciiChar = text[i];
		m_buffer[offset].Attributes = attributes;
	}
}

void Window::FillLine(int y, char c, WORD attributes)
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

void Window::Flush(HANDLE handle)
{
	assert(m_buffer != 0);
	assert(m_width > 0);
	assert(m_height > 0);

    COORD bufferSize;
    bufferSize.X = m_width;
    bufferSize.Y = m_height;

    COORD bufferCoord;
    bufferCoord.X = 0;
    bufferCoord.Y = 0;

    SMALL_RECT rect;
    rect.Top = m_y;
    rect.Left = m_x;
    rect.Bottom = m_height - 1;
    rect.Right = m_width - 1;

    WriteConsoleOutput(handle, m_buffer, bufferSize, bufferCoord, &rect);
}

void Window::OnWindowResize(int width, int height)
{
	delete[] m_buffer;
	m_width = width;
	m_height = height;
	m_buffer = new CHAR_INFO[width * height];
}
