#include "Window.h"
#include <assert.h>

std::vector<Window*> Window::s_windows;
ConsoleBuffer* Window::s_consoleBuffer;

Window::Window()
{
	m_width = 0;
	m_height = 0;
    Add(this);
}

Window::~Window()
{
    Remove(this);
}

void Window::OnWindowResized(int width, int height)
{
    m_width = width;
    m_height = height;
}
