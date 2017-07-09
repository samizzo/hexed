#include "MainWindow.h"
#include "Path.h"
#include "Colours.h"
#include <assert.h>

static const int MAX_FILENAME_SIZE = 32;

MainWindow::MainWindow(const char* filename) :
m_hexView(filename)
{
    m_helpWindow.SetVisible(false);

    GetFullPathName(filename, MAX_PATH, m_fullPath, &m_filename);

    // If full path is too long, just show the filename.
    if (strlen(m_fullPath) > MAX_FILENAME_SIZE)
    {
        // If filename is too long, truncate it.
        int len = strlen(m_filename);
        if (len <= MAX_FILENAME_SIZE)
        {
            strncpy_s(m_fullPath, MAX_PATH, m_filename, len);
        }
        else
        {
            len = MAX_FILENAME_SIZE;
            strncpy_s(m_fullPath, MAX_PATH, m_filename, len);
            strcat_s(m_fullPath, MAX_PATH, "..");
        }
    }
}

void MainWindow::OnWindowRefreshed()
{
    s_consoleBuffer->Clear(Colours::Background);
    s_consoleBuffer->FillLine(0, ' ', Colours::StatusBar);
    s_consoleBuffer->Write(1, 0, Colours::StatusBar, m_fullPath);
    s_consoleBuffer->FillLine(m_height - 1, ' ', Colours::StatusBar);

    int selectedOffset = m_hexView.GetSelectedOffset();
    int fileSize = max(m_hexView.GetFileSize() - 1, 0);
    s_consoleBuffer->Write(m_width - 20, 0, Colours::StatusBar, "%08X / %08X", selectedOffset, fileSize);

    s_consoleBuffer->Write(0, m_height - 1, Colours::FunctionButton,  " F1 Help    ");
    //s_consoleBuffer->Write(14, m_height - 1, BACKGROUND_GREEN | BACKGROUND_RED, " F2 Colours ");
}

void MainWindow::OnKeyEvent(KeyEvent& keyEvent)
{
    unsigned short vkKeyCode = keyEvent.GetVKKeyCode();
    switch (vkKeyCode)
    {
        case VK_F1:
        {
            if (!keyEvent.IsKeyDown())
            {
                m_helpWindow.SetVisible(true);
            }

            break;
        }
    }
}
