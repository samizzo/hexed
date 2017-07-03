#include "HexView.h"
#include "Log.h"

HexView::HexView(ConsoleBuffer* consoleBuffer) : Window(consoleBuffer)
{
}

void HexView::Draw()
{
    m_consoleBuffer->FillRect(0, 1, m_width, m_height, '.', FOREGROUND_RED);
}

void HexView::OnWindowResize(int newWidth, int newHeight)
{
    newHeight -= 2;
    LogDebug("width=%i, height=%i\n", newWidth, newHeight);
    Window::OnWindowResize(newWidth, newHeight);
}
