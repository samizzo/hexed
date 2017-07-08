#include "HelpWindow.h"

void HelpWindow::OnWindowRefreshed()
{
	WORD colour = FOREGROUND_INTENSITY | BACKGROUND_GREEN;
	s_consoleBuffer->DrawWindow(m_x, m_y, m_width, m_height, colour);
}

void HelpWindow::OnWindowResized(int width, int height)
{
    int newWidth = (int)(width * 0.8f);
    int newHeight = (int)(height * 0.8f);
    Window::OnWindowResized(newWidth, newHeight);

    m_x = (width - newWidth) >> 1;
    m_y = (height - newHeight) >> 1;
}

void HelpWindow::OnKeyEvent(KeyEvent& keyEvent)
{
    keyEvent.SetHandled();
    if (!keyEvent.IsKeyDown() && keyEvent.GetVKKeyCode() == VK_ESCAPE)
        SetVisible(false);
}
