#include "Window.h"
#include <assert.h>
#include "Path.h"

std::vector<Window*> Window::s_windows;
ConsoleBuffer* Window::s_consoleBuffer;

Window::Window(const char* filename)
{
    m_filename = Path::FindFileName(filename);
	m_width = 0;
	m_height = 0;
    Add(this);
}

Window::~Window()
{
    Remove(this);
}

void Window::OnWindowRefreshed()
{
	assert(s_consoleBuffer != 0);

    s_consoleBuffer->Clear();
	s_consoleBuffer->FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
	s_consoleBuffer->Write(2, 0, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, m_filename);
}

void Window::OnWindowResized(int width, int height)
{
    m_width = width;
    m_height = height;
}
