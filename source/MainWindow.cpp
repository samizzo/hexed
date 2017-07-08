#include "MainWindow.h"
#include "Path.h"
#include <assert.h>

MainWindow::MainWindow(const char* filename)
{
    m_filename = Path::FindFileName(filename);
}

void MainWindow::OnWindowRefreshed()
{
    s_consoleBuffer->Clear();
    s_consoleBuffer->FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    s_consoleBuffer->Write(2, 0, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, m_filename);

    Window::OnWindowRefreshed();
}
