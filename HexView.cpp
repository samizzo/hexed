#include "HexView.h"
#include "Log.h"
#include <stdio.h>
#include <assert.h>

HexView::HexView(ConsoleBuffer* consoleBuffer, const char* filename) : Window(consoleBuffer, filename)
{
    m_fp = 0;
    m_buffer = 0;
    m_offset = 0;
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

void HexView::Draw()
{
    m_consoleBuffer->FillRect(0, 1, m_width, m_height, ' ', FOREGROUND_RED);

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
        m_consoleBuffer->Write(1, y, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%08X", offset);
        x += 9;

        for (int i = 0; i < 16; i++, offset++)
        {
            if (offset >= m_bufferSize)
                break;

            unsigned char c = m_buffer[offset];

            int xx = x + (i * 3);
            m_consoleBuffer->Write(xx, y, FOREGROUND_GREEN, "%02X", c);

            xx = x + (16 * 3) + i;
            if (!isprint(c))
                c = '.';
            m_consoleBuffer->Write(xx, y, FOREGROUND_GREEN, "%c", c);
        }
    }
}

void HexView::OnWindowResize(int width, int height)
{
    height -= 2;
    Window::OnWindowResize(width, height);
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
