#include "Window.h"
#include <assert.h>

Window::Window(ConsoleBuffer* consoleBuffer)
{
	m_width = 0;
	m_height = 0;
    m_consoleBuffer = consoleBuffer;
    m_x = 0;
    m_y = 0;
}

void Window::Draw()
{
	assert(m_consoleBuffer != 0);

    m_consoleBuffer->Clear();
	m_consoleBuffer->FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
	m_consoleBuffer->Write(2, 0, "hexed.exe", BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
	m_consoleBuffer->FillLine(m_height - 1, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
}

void Window::OnWindowResize(int width, int height)
{
    m_width = width;
    m_height = height;
}
