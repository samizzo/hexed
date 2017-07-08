#include "MainWindow.h"
#include "Path.h"
#include <assert.h>

MainWindow::MainWindow(const char* filename) :
m_hexView(filename, this)
{
    m_filename = Path::FindFileName(filename);
}

void MainWindow::OnWindowRefreshed()
{
    s_consoleBuffer->Clear();
    s_consoleBuffer->FillLine(0, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    s_consoleBuffer->Write(2, 0, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, m_filename);
    s_consoleBuffer->FillLine(m_height - 1, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);

    int selectedOffset = m_hexView.GetSelectedOffset();
    int fileSize = max(m_hexView.GetFileSize() - 1, 0);
    s_consoleBuffer->Write(2, m_height - 1, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, "%08X / %08X", selectedOffset, fileSize);

    Window::OnWindowRefreshed();
}
