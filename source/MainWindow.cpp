#include "MainWindow.h"
#include "Path.h"
#include "Colours.h"
#include <assert.h>

MainWindow::MainWindow(File* file) :
    m_hexView(file)
{
    m_file = file;
    m_helpWindow.SetVisible(false);
}

void MainWindow::OnWindowRefreshed()
{
    // If full path is too long, just show the filename.
    unsigned int maxFilename = m_width - 1 - 8 - 3 - 8 - 1 - 4 - 1 - 1; // Header looks like: " <filename> [RO] xxxxxxxx / yyyyyyyy "
    const char* fullPath = m_file->GetFullPath();
    if (strlen(fullPath) > maxFilename)
    {
        // If filename is too long, truncate it.
        const char* fileName = m_file->GetFileName();
        size_t len = strlen(fileName);
        if (len <= maxFilename)
        {
            strncpy_s(m_filename, MAX_PATH, fileName, len);
        }
        else
        {
            len = maxFilename - 2;
            strncpy_s(m_filename, MAX_PATH, fileName, len);
            strcat_s(m_filename, MAX_PATH, "..");
        }
    }
    else
    {
        strncpy_s(m_filename, MAX_PATH, fullPath, _TRUNCATE);
    }

    s_consoleBuffer->Clear(Colours::Background);
    s_consoleBuffer->FillLine(0, ' ', Colours::StatusBar);
    s_consoleBuffer->Write(1, 0, Colours::StatusBar, m_filename);
    if (m_file->IsReadOnly())
    {
        size_t readOnlyOffset = strlen(m_filename) + 2;
        s_consoleBuffer->Write((int)readOnlyOffset, 0, Colours::StatusBar, "[RO]");
    }

    s_consoleBuffer->FillLine(m_height - 1, ' ', Colours::StatusBar);

    int selectedOffset = m_hexView.GetSelectedOffset();
    int fileSize = max(m_hexView.GetFileSize() - 1, 0);
    s_consoleBuffer->Write(m_width - 20, 0, Colours::StatusBar, "%08X / %08X", selectedOffset, fileSize);

    //s_consoleBuffer->Write(0, m_height - 1, Colours::FunctionButton,  " F1 Help    ");
}

void MainWindow::OnKeyEvent(KeyEvent& keyEvent)
{
    unsigned short vkKeyCode = keyEvent.GetVKKeyCode();
    switch (vkKeyCode)
    {
        case VK_F1:
        {
            /*if (!keyEvent.IsKeyDown())
            {
                m_helpWindow.SetVisible(true);
            }*/

            break;
        }
    }
}
