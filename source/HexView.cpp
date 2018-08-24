#include "HexView.h"
#include "Log.h"
#include "Colours.h"
#include <assert.h>

static const int BYTES_OFFSET = 11;
static const int CHARS_OFFSET = BYTES_OFFSET + (16 * 3);

HexView::HexView(File* file)
{
	m_mode = Mode_None;
    m_buffer = 0;
    m_topLine = 0;
    m_selected = 0;
    m_fileSize = 0;
	m_file = file;
	assert(file);
    if (m_file->IsOpen())
        m_fileSize = m_file->GetSize();
}

HexView::~HexView()
{
    m_file->Close();
}

void HexView::OnWindowRefreshed()
{
    if (!m_file->IsOpen())
        return;

    assert(m_buffer);
    int offset = m_topLine << 4;
    int selectedLine = GetSelectedLine();
    bool done = false;

    for (int j = 0; j < m_height; j++)
    {
        int y = 1 + j;
        WORD colour = 0;

		int lastLine = (m_fileSize - 1) >> 4;
		int curr = lastLine == 0 ? 0 : GetSelectedLine() * (m_height - 1) / lastLine;
        char c = j == curr ? 178 : 176;
        s_consoleBuffer->Write(0, y, Colours::Scrollbar, "%c", c);

        if (done)
            continue;

        if ((offset >> 4) == selectedLine)
        {
            // Highlight the selected line's offset text.
			colour = Colours::HexViewHighlight;
        }
        else
        {
			colour = Colours::HexViewOffsetNormal;
        }

        s_consoleBuffer->Write(2, y, colour, "%08X", offset);

        int x = BYTES_OFFSET;

        for (int i = 0; i < 16; i++, offset++)
        {
            int bufferIndex = offset - (m_topLine << 4);
            assert(bufferIndex >= 0 && bufferIndex < m_fileSize);
            unsigned char c = m_buffer[bufferIndex];

            if (offset == m_selected && (m_mode == Mode_None || m_mode == Mode_EditChar))
            {
                // Highlight the selected byte but only if not in edit mode,
				// or if we are in edit mode, then only if we are not editing
				// the bytes.
				colour = Colours::HexViewHighlight;
            }
            else
            {
				colour = Colours::HexViewByteNormal;
            }

            int xx = x + (i * 3);
            s_consoleBuffer->Write(xx, y, colour, "%02X", c);

            xx = CHARS_OFFSET + i;
            if (c < ' ')
                c = '.';

            if (offset == m_selected && (m_mode == Mode_None || m_mode == Mode_EditByte))
            {
                // Highlight the selected character but only if not in edit mode,
				// or if we are in edit mode, then only if we are not editing the
				// bytes.
				colour = Colours::HexViewHighlight;
            }
            else
            {
				colour = Colours::HexViewCharNormal;
            }
            s_consoleBuffer->Write(xx, y, colour, "%c", c);

			if ((offset + 1) >= m_fileSize)
			{
				done = true;
				break;
			}
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
    if (!m_file->IsOpen())
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

    m_file->Seek(offset);
    m_file->Read(m_buffer, m_bufferSize);
}

void HexView::OnKeyEvent(KeyEvent& keyEvent)
{
	if (!keyEvent.IsKeyDown())
	{
		switch (keyEvent.GetVKKeyCode())
		{
			case VK_ESCAPE:
			{
				if (m_mode == Mode_None)
					break;

				// Escape when in edit mode cancels the edit mode.
				keyEvent.SetHandled();
				m_mode = Mode_None;
				s_consoleBuffer->SetCursor(false, 1);
				Window::Refresh(true);
				break;
			}
		}

		return;
	}

	bool refresh = true;
	bool fullDraw = false;

	unsigned short vkCode = keyEvent.GetVKKeyCode();

	if (m_mode == Mode_None)
	{
		unsigned char ascii = keyEvent.GetAscii();
		if (ascii == '/' || (tolower(ascii) == 'f' && keyEvent.IsControlKeyDown(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
		{
			// Forward slash or ctrl-f switch to find mode.
			m_mode = Mode_Find;
		}
	}

	if (m_mode == Mode_Find)
	{
		s_consoleBuffer->SetCursor(true, 100);
		s_consoleBuffer->SetCursorPosition(1, m_height + 1);
	}
	else
	{
		if (m_mode == Mode_EditByte)
		{
			unsigned char ascii = toupper(keyEvent.GetAscii());
			if ((ascii >= 'A' && ascii <= 'F') || (ascii >= '0' && ascii <= '9'))
			{
				WriteBytes(ascii);
				vkCode = VK_RIGHT;
			}
		}
		else if (m_mode == Mode_EditChar)
		{
			unsigned char ascii = keyEvent.GetAscii();
			if (ascii >= 32 && ascii < 127)
			{
				WriteChar((unsigned char)ascii);
				vkCode = VK_RIGHT;
			}
		}

		switch (vkCode)
		{
			case VK_LEFT:
			{
				if (m_mode == Mode_EditByte && m_nibbleIndex == 0)
				{
					// In byte edit mode
					m_nibbleIndex = 1;
				}
				else
				{
					m_nibbleIndex = 0;
					m_selected = max(m_selected - 1, 0);
					int selectedLine = GetSelectedLine();
					if (selectedLine < m_topLine)
					{
						m_topLine--;
						assert(m_topLine >= 0);
						fullDraw = true;
						CacheFile();
					}
				}
				break;
			}

			case VK_RIGHT:
			{
				if (m_mode == Mode_EditByte && m_nibbleIndex == 1)
				{
					m_nibbleIndex = 0;
				}
				else
				{
					m_nibbleIndex = 1;
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

			case VK_INSERT:
			{
				if (m_mode != Mode_None || !m_file->IsOpen())
					break;

				m_mode = Mode_EditByte;
				m_nibbleIndex = 1;
				s_consoleBuffer->SetCursor(true, 100);
				break;
			}

			case VK_TAB:
			{
				if (m_mode == Mode_None)
					break;

				m_mode = m_mode == Mode_EditByte ? Mode_EditChar : Mode_EditByte;
				break;
			}
		}
	}

	if (refresh)
	{
		Window::Refresh(fullDraw);
		UpdateCursor();
	}
}

void HexView::UpdateCursor()
{
	if (m_mode != Mode_EditByte && m_mode != Mode_EditChar)
		return;

	HANDLE stdoutHandle = s_consoleBuffer->GetStdoutHandle();
	int x, y;

	if (m_mode == Mode_EditByte)
	{
		x = BYTES_OFFSET + ((m_selected & 0xf) * 3) + (m_nibbleIndex ^ 1);
	}
	else
	{
		x = CHARS_OFFSET + (m_selected & 0xf);
	}

	y = (GetSelectedLine() - m_topLine) + 1;
	s_consoleBuffer->SetCursorPosition(x, y);
}

void HexView::WriteBytes(unsigned char ascii)
{
	// Write bytes to the buffer.
	int bufferIndex = m_selected - (m_topLine << 4);
	assert(bufferIndex >= 0 && bufferIndex < m_bufferSize);

	char value = (ascii >= 'A' && ascii <= 'F') ? ascii - 'A' + 10 : ascii - '0';
	unsigned char b = m_buffer[bufferIndex];
	unsigned char mask = 0xf << (4 * m_nibbleIndex);
	b = (b & ~mask) | (value << (4 * m_nibbleIndex));
	m_buffer[bufferIndex] = b;

	m_file->Seek(m_selected);
	m_file->Write(m_buffer + bufferIndex, 1);
}

void HexView::WriteChar(unsigned char ascii)
{
	// Write chars to the buffer.
	int bufferIndex = m_selected - (m_topLine << 4);
	assert(bufferIndex >= 0 && bufferIndex < m_bufferSize);
	m_buffer[bufferIndex] = ascii;
	m_file->Seek(m_selected);
	m_file->Write(m_buffer + bufferIndex, 1);
}
