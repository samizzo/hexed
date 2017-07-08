#include "HexView.h"
#include "Log.h"
#include <assert.h>

HexView::HexView(const char* filename, Window* parent) : Window(parent)
{
    m_buffer = 0;
    m_topLine = 0;
    m_selected = 0;
    m_fileSize = 0;
    m_file.Open(filename);
    if (m_file.IsOpen())
        m_fileSize = m_file.GetSize();
}

HexView::~HexView()
{
    m_file.Close();
}

void HexView::OnWindowRefreshed()
{
    Window::OnWindowRefreshed();

    if (!m_file.IsOpen())
        return;

    assert(m_buffer);
    int offset = m_topLine << 4;
    int selectedLine = GetSelectedLine();
    bool done = false;

    for (int j = 0; j < m_height; j++)
    {
        int y = 1 + j;
        int x = 2;
        WORD colour = 0;

        int curr = (m_selected >> 4) * (m_height - 1) / ((m_fileSize - 1) >> 4);
        char c = j == curr ? 178 : 176;
        s_consoleBuffer->Write(0, y, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "%c", c);

        if (done)
            continue;

        if ((offset >> 4) == selectedLine)
        {
            // Highlight the selected line's offset text.
            colour = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
        }
        else
        {
            colour = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        }

        s_consoleBuffer->Write(x, y, colour, "%08X", offset);
        x += 9;

        for (int i = 0; i < 16; i++, offset++)
        {
            int bufferIndex = offset - (m_topLine << 4);
            if (offset >= m_fileSize)
            {
                done = true;
                break;
            }

            assert(bufferIndex >= 0 && bufferIndex < m_fileSize);
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
    // Our height is actually smaller.
    height -= 2;
    Window::OnWindowResized(width, height);
    CacheFile(true);
}

void HexView::CacheFile(bool resizeBuffer)
{
    if (!m_file.IsOpen())
        return;

    assert(m_topLine >= 0);
    int offset = m_topLine << 4;
    assert(offset >= 0 && offset < m_fileSize);

    if (resizeBuffer)
    {
        assert(m_height >= 0);
        int screenSize = m_height << 4;

        delete[] m_buffer;
        m_bufferSize = offset + screenSize >= m_fileSize ? m_fileSize - offset : screenSize;
        m_buffer = new unsigned char[m_bufferSize];
        memset(m_buffer, 0, m_bufferSize);
    }

    m_file.Seek(offset);
    m_file.Read(m_buffer, m_bufferSize);
}

void HexView::OnKeyEvent(KeyEvent& keyEvent)
{
    Window::OnKeyEvent(keyEvent);

    if (!keyEvent.IsKeyDown())
        return;

    bool refresh = true;
    bool fullDraw = false;

    switch (keyEvent.GetVKKeyCode())
    {
        case VK_LEFT:
        {
            m_selected = max(m_selected - 1, 0);
            int selectedLine = GetSelectedLine();
            if (selectedLine < m_topLine)
            {
                m_topLine--;
                assert(m_topLine >= 0);
                fullDraw = true;
                CacheFile();
            }
            break;
        }

        case VK_RIGHT:
        {
            m_selected = max(min(m_selected + 1, m_fileSize - 1), 0);
            int selectedLine = GetSelectedLine();
            int bottomLine = GetBottomLine();
            if (selectedLine > bottomLine)
            {
                m_topLine++;
                assert((m_topLine << 4) < m_fileSize);
                fullDraw = true;
                CacheFile();
            }
            break;
        }

        case VK_UP:
        {
            refresh = false;
            int selectedLine = GetSelectedLine();
            if (selectedLine == 0)
                break;

            refresh = true;
            m_selected = max(m_selected - 16, 0);
            selectedLine = GetSelectedLine();
            if (selectedLine < m_topLine)
            {
                m_topLine--;
                assert(m_topLine >= 0);
                fullDraw = true;
                CacheFile();
            }
            break;
        }

        case VK_DOWN:
        {
            refresh = false;
            int selectedLine = GetSelectedLine();
            int lastLine = GetLastLine();

            // If on the last line, don't move anywhere.
            if (selectedLine == lastLine)
                break;

            refresh = true;
            m_selected = max(min(m_selected + 16, m_fileSize - 1), 0);
            selectedLine = GetSelectedLine();

            int bottomLine = GetBottomLine();
            if (selectedLine > bottomLine)
            {
                m_topLine++;
                assert((m_topLine << 4) < m_fileSize);
                fullDraw = true;
                CacheFile();
            }
            break;
        }

        case VK_HOME:
        {
            if (keyEvent.IsControlKeyDown(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
            {
                m_selected = 0;
                m_topLine = 0;
                fullDraw = true;
                CacheFile();
            }
            else
            {
                m_selected &= ~15;
            }
            break;
        }

        case VK_END:
        {
            if (keyEvent.IsControlKeyDown(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
            {
                m_selected = max(m_fileSize - 1, 0);
                int selectedLine = GetSelectedLine();
                m_topLine = max(selectedLine - m_height + 1, 0);
                fullDraw = true;
                CacheFile();
            }
            else
            {
                m_selected = max(min(m_selected | 15, m_fileSize - 1), 0);
            }
            break;
        }

        // Page down.
        case VK_NEXT:
        {
            // Current selection column in the last line.
            int lastLineSelected = max(min(((m_fileSize - 1) & ~0xf) | (m_selected & 0xf), m_fileSize - 1), 0);

            if (keyEvent.IsControlKeyDown(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
            {
                // Control is down. Go to the bottom of the current page.
                int bottomLine = GetBottomLine();

                // Select the offset at the bottom of the current page.
                m_selected = min((bottomLine << 4) | (m_selected & 0xf), lastLineSelected);
            }
            else
            {
                int selectedLine = GetSelectedLine();

                // Get the current distance from the selection to the top line.
                int delta = selectedLine - m_topLine;

                // Select the offset at one page down from the current.
                m_selected = min(m_selected + (m_height << 4), lastLineSelected);

                // Determine if we need to update the top line.
                selectedLine = GetSelectedLine();
                int bottomLine = GetBottomLine();
                if (selectedLine > bottomLine)
                {
                    // Update the top line, but maintain the current selection distance so the cursor
                    // never moves.
                    m_topLine = max(selectedLine - delta, 0);
                    fullDraw = true;
                    CacheFile();
                }
            }
            break;
        }

        // Page up.
        case VK_PRIOR:
        {
            if (keyEvent.IsControlKeyDown(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
            {
                // Control is down. Go to the top of the current page.
                // Select the offset at the top of the current page.
                m_selected = (m_topLine << 4) | m_selected & 0xf;
            }
            else
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
                    fullDraw = true;
                    CacheFile();
                }
            }
            break;
        }

        case VK_F5:
        {
            fullDraw = true;
            break;
        }
    }

    if (refresh)
        Window::Refresh(fullDraw);
}
