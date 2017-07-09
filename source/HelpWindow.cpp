#include "HelpWindow.h"

void HelpWindow::OnWindowRefreshed()
{
	WORD colour = FOREGROUND_INTENSITY | BACKGROUND_GREEN;
	s_consoleBuffer->DrawWindow(m_x, m_y, m_width, m_height, colour);

	s_consoleBuffer->Write(m_x + 10, m_y + 8, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN, "Foreground");
	s_consoleBuffer->Write(m_x + 30, m_y + 8, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN, "Background");
	for (int i = 0; i < 16; i++)
	{
		int x = (m_x + 10) + ((i >> 3) * 10);
		int y = m_y + 10 + (i & 7);
		s_consoleBuffer->Write(x, y, i, "%c%c%c%c%c%c%c%c", 219, 219, 219, 219, 219, 219, 219, 219);

		s_consoleBuffer->Write(x + 20, y, i << 4, "        ");
	}
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
