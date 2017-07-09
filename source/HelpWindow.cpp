#include "HelpWindow.h"
#include "Colours.h"

void HelpWindow::OnWindowRefreshed()
{
	WORD colour = COLOUR(7, 3);
	s_consoleBuffer->DrawWindow(m_x, m_y, m_width, m_height, colour);

	/*s_consoleBuffer->Write(m_x + 10, m_y + 8, COLOUR(15, 3), "Colours");
	for (int i = 0; i < 16; i++)
	{
		int x = (m_x + 10) + ((i >> 3) * 10);
		int y = m_y + 10 + (i & 7);
		s_consoleBuffer->Write(x, y, i, "%c%c%c%c%c%c%c%c", 219, 219, 219, 219, 219, 219, 219, 219);
	}*/
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
