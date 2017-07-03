#include "HexView.h"
#include "Log.h"
#include <stdio.h>
#include <assert.h>

HexView::HexView(const char* filename) : Window(filename)
{
    m_fp = 0;
    m_buffer = 0;
    m_offset = 0;
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

    if (!m_fp)
        return;

    assert(m_buffer);
    int offset = m_offset;

    for (int j = 0; j < m_height; j++)
    {
        if (offset >= m_bufferSize)
            break;

        int y = 1 + j;
        int x = 1;
        WORD colour = 0;
        if (offset >> 4 == m_selected >> 4)
        {
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
            if (offset >= m_bufferSize)
                break;

            unsigned char c = m_buffer[offset];

            if (offset == m_selected)
            {
                colour = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            }
            else
            {
                colour = FOREGROUND_GREEN;
            }

            int xx = x + (i * 3);
            s_consoleBuffer->Write(xx, y, colour, "%02X", c);

            xx = x + (16 * 3) + i;
            if (!isprint(c))
                c = '.';

            if (offset == m_selected)
            {
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

    m_bufferSize = m_offset + (m_height * 16) > m_fileSize ? m_fileSize - m_offset : m_height * 16;
    m_buffer = new unsigned char[m_bufferSize];
    memset(m_buffer, 0, m_bufferSize);

    fseek(m_fp, m_offset, SEEK_SET);
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
            Window::Refresh();
            break;
        }

        case VK_RIGHT:
        {
            m_selected = min(m_selected + 1, m_fileSize - 1);
            Window::Refresh();
            break;
        }

        case VK_UP:
        {
            if (m_selected >> 4 != 0)
            {
                m_selected = max(m_selected - 16, 0);
                Window::Refresh();
            }
            break;
        }

        case VK_DOWN:
        {
            if (m_selected >> 4 != m_fileSize >> 4)
            {
                m_selected = min(m_selected + 16, m_fileSize - 1);
                Window::Refresh();
            }
            break;
        }

        case VK_HOME:
        {
            m_selected = 0;
            Window::Refresh();
            break;
        }

        case VK_END:
        {
            m_selected = m_fileSize - 1;
            Window::Refresh();
            break;
        }
    }
}
