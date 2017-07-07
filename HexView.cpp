#include "HexView.h"
#include "Log.h"
#include <stdio.h>
#include <assert.h>

HexView::HexView(const char* filename) : Window(filename)
{
    m_fp = 0;
    m_buffer = 0;
    m_topLine = 0;
    m_selected = 0;
    fopen_s(&m_fp, filename, "rb");
    if (m_fp)
    {
        fseek(m_fp, 0, SEEK_END);
        m_fileSize = ftell(m_fp);
        fseek(m_fp, 0, SEEK_SET);
    }
}

HexView::~HexView()
{
    if (m_fp)
        fclose(m_fp);
}

void HexView::OnWindowRefreshed()
{
    s_consoleBuffer->FillRect(0, 1, m_width, m_height, ' ', FOREGROUND_RED);
    s_consoleBuffer->FillLine(m_height + 1, ' ', BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    s_consoleBuffer->Write(2, m_height + 1, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, "%08X / %08X", m_selected, m_fileSize - 1);

    if (!m_fp)
        return;

    assert(m_buffer);
    int offset = m_topLine << 4;
    int selectedLine = GetSelectedLine();
    bool done = false;

    for (int j = 0; j < m_height && !done; j++)
    {
        int y = 1 + j;
        int x = 1;
        WORD colour = 0;

        if ((offset >> 4) == selectedLine)
        {
            // Highlight the selected line's offset text.
            colour = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY | COMMON_LVB_REVERSE_VIDEO;
        }
        else
        {
            colour = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        }

        s_consoleBuffer->Write(1, y, colour, "%08X", offset);
        x += 9;

        for (int i = 0; i < 16; i++, offset++)
        {
            int bufferIndex = offset - (m_topLine << 4);
            if (bufferIndex >= m_bufferSize)
            {
                done = true;
                break;
            }

            unsigned char c = m_buffer[bufferIndex];

            if (offset == m_selected)
            {
                // Highlight the selected byte.
                colour = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            }
            else
            {
                colour = FOREGROUND_GREEN;
            }

            int xx = x + (i * 3);
            s_consoleBuffer->Write(xx, y, colour, "%02X", c);

            xx = x + (16 * 3) + i;
            if (c < ' ')
                c = '.';

            if (offset == m_selected)
            {
                // Highlight the selected character.
                colour = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            }
            else
            {
                colour = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            s_consoleBuffer->Write(xx, y, colour, "%c", c);
        }
    }
}

void HexView::OnWindowResized(int width, int height)
{
    height -= 2;
    Window::OnWindowResized(width, height);
    CacheFile();
}

void HexView::CacheFile()
{
    if (!m_fp)
        return;

    delete[] m_buffer;

    assert(m_topLine >= 0);
    int offset = m_topLine << 4;
    assert(offset >= 0 && offset < m_fileSize);
    assert(m_height >= 0);
    int screenSize = m_height << 4;

    // Reallocate the buffer.
    // TODO: Only do this if the window size changes!
    m_bufferSize = offset + screenSize >= m_fileSize ? m_fileSize - offset : screenSize;
    m_buffer = new unsigned char[m_bufferSize];
    memset(m_buffer, 0, m_bufferSize);

    fseek(m_fp, offset, SEEK_SET);
    fread_s(m_buffer, m_bufferSize, 1, m_bufferSize, m_fp);
}

void HexView::OnKeyEvent(const KEY_EVENT_RECORD& ker)
{
    if (!ker.bKeyDown)
        return;

    switch (ker.wVirtualKeyCode)
    {
        case VK_LEFT:
        {
            m_selected = max(m_selected - 1, 0);
            int selectedLine = GetSelectedLine();
            if (selectedLine < m_topLine)
            {
                m_topLine--;
                assert(m_topLine >= 0);
                CacheFile();
            }

            Window::Refresh();
            break;
        }

        case VK_RIGHT:
        {
            m_selected = min(m_selected + 1, m_fileSize - 1);
            int selectedLine = GetSelectedLine();
            int bottomLine = GetBottomLine();
            if (selectedLine > bottomLine)
            {
                m_topLine++;
                assert((m_topLine << 4) < m_fileSize);
                CacheFile();
            }

            Window::Refresh();
            break;
        }

        case VK_UP:
        {
            int selectedLine = GetSelectedLine();
            if (selectedLine == 0)
                break;

            m_selected = max(m_selected - 16, 0);
            selectedLine = GetSelectedLine();
            if (selectedLine < m_topLine)
            {
                m_topLine--;
                assert(m_topLine >= 0);
                CacheFile();
            }

            Window::Refresh();
            break;
        }

        case VK_DOWN:
        {
            int selectedLine = GetSelectedLine();
            int lastLine = GetLastLine();

            // If on the last line, don't move anywhere.
            if (selectedLine == lastLine)
                break;

            m_selected = min(m_selected + 16, m_fileSize - 1);
            selectedLine = GetSelectedLine();

            int bottomLine = GetBottomLine();
            if (selectedLine > bottomLine)
            {
                m_topLine++;
                assert((m_topLine << 4) < m_fileSize);
                CacheFile();
            }

            Window::Refresh();
            break;
        }

        case VK_HOME:
        {
            DWORD ctrl = ker.dwControlKeyState;
            if ((ctrl & LEFT_CTRL_PRESSED) || (ctrl & RIGHT_CTRL_PRESSED))
            {
                m_selected = 0;
                m_topLine = 0;
                CacheFile();
            }
            else
            {
                m_selected &= ~15;
            }

            Window::Refresh();
            break;
        }

        case VK_END:
        {
            DWORD ctrl = ker.dwControlKeyState;
            if ((ctrl & LEFT_CTRL_PRESSED) || (ctrl & RIGHT_CTRL_PRESSED))
            {
                m_selected = m_fileSize - 1;
                int selectedLine = GetSelectedLine();
                m_topLine = max(selectedLine - m_height + 1, 0);
                CacheFile();
            }
            else
            {
                m_selected = min(m_selected | 15, m_fileSize - 1);
            }

            Window::Refresh();
            break;
        }

        // Page down.
        case VK_NEXT:
        {
            int selectedLine = GetSelectedLine();

            DWORD ctrl = ker.dwControlKeyState;
            if ((ctrl & LEFT_CTRL_PRESSED) || (ctrl & RIGHT_CTRL_PRESSED))
            {
                // Control is down. Go to the bottom of the current page.
                int bottomLine = GetBottomLine();

                // Select the offset at the bottom of the current page. If we reach the end of the
                // file then select the same column in the last line, unless it's outside the bounds
                // of the file in which case clamp to the end.
                m_selected = min((bottomLine << 4) | (m_selected & 0xf), ((m_fileSize - 1) & ~0xf) | (m_selected & 0xf));
                m_selected = min(m_selected, m_fileSize - 1);
            }
            else
            {
                // Get the current distance from the selection to the top line.
                int delta = selectedLine - m_topLine;

                // Select the offset at one page down from the current. If we reach the end of the
                // file then select the same column in the last line, unless it's outside the bounds
                // of the file in which case clamp to the end.
                m_selected = min(m_selected + (m_height << 4), ((m_fileSize - 1) & ~0xf) | (m_selected & 0xf));
                m_selected = min(m_selected, m_fileSize - 1);

                // Determine if we need to update the top line.
                selectedLine = GetSelectedLine();
                int bottomLine = GetBottomLine();
                if (selectedLine > bottomLine)
                {
                    // Update the top line, but maintain the current selection distance so the cursor
                    // never moves.
                    m_topLine = max(selectedLine - delta, 0);
                    CacheFile();
                }
            }

            Window::Refresh();
            break;
        }

        // Page up.
        case VK_PRIOR:
        {
            int selectedLine = GetSelectedLine();

            // Get the current distance from the selection to the top line.
            int delta = selectedLine - m_topLine;

            // Select the offset at one page up from the current.
            m_selected = max(m_selected - (m_height << 4), (m_selected & 0xf));

            // Determine if we need to update the top line.
            selectedLine = GetSelectedLine();
            if (selectedLine < m_topLine)
            {
                // Update the top line, but maintain the current selection distance so the cursor
                // never moves.
                m_topLine = max(selectedLine - delta, 0);
                CacheFile();
            }

            Window::Refresh();
            break;
        }
    }
}
