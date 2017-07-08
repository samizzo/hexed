#include "HelpWindow.h"

HelpWindow::HelpWindow(Window* parent) :
Window(parent)
{
}

void HelpWindow::OnWindowRefreshed()
{
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            s_consoleBuffer->Write(x + m_x, y + m_y, FOREGROUND_BLUE, "%c", 219);
        }
    }
}

void HelpWindow::OnWindowResized(int width, int height)
{
    int newWidth = width >> 1;
    int newHeight = height >> 1;
    Window::OnWindowResized(newWidth, newHeight);

    m_x = (width - newWidth) >> 1;
    m_y = (height - newHeight) >> 1;
}

void HelpWindow::OnKeyEvent(KeyEvent& keyEvent)
{
    if (!keyEvent.IsKeyDown() && keyEvent.GetVKKeyCode() == VK_ESCAPE)
        SetVisible(false);
}
