#include "HexView.h"

void HexView::Draw()
{
    //Window::Draw();
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_buffer[i].Attributes = FOREGROUND_RED;
        m_buffer[i].Char.AsciiChar = '.';
    }
}

void HexView::OnWindowResize(int newWidth, int newHeight)
{
    newHeight--;
    Window::OnWindowResize(newWidth, newHeight);
}
