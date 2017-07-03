#include "Window.h"
#include <assert.h>
#include "Path.h"

Window::Window(ConsoleBuffer* consoleBuffer, const char* filename)
{
    m_filename = filename;
	m_width = 0;
	m_height = 0;
    m_consoleBuffer = consoleBuffer;
}

void Window::Draw()
{
	assert(m_consoleBuffer != 0);

    m_consoleBuffer->Clear();
	m_consoleBuffer->FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    const char* filename = Path::FindFileName(m_filename);
	m_consoleBuffer->Write(2, 0, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, filename);
	m_consoleBuffer->FillLine(m_height - 1, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
}

void Window::OnWindowResize(int width, int height)
{
    m_width = width;
    m_height = height;
}
