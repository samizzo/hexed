#pragma once

#include "Window.h"
#include "File.h"

class HexView : public Window
{
    public:
        HexView(const char* filename);
        ~HexView();

        virtual void OnWindowRefreshed();
        virtual void OnWindowResized(int newWidth, int newHeight);
        virtual void OnKeyEvent(KeyEvent& keyEvent);

        int GetSelectedOffset();
        int GetFileSize();

    private:
		void UpdateCursor();
        void CacheFile(bool resizeBuffer = false);
		void WriteBytes(unsigned char ascii);
		void WriteChar(unsigned char ascii);

        int GetSelectedLine();
        int GetLastLine();
        int GetBottomLine();

        File m_file;
        unsigned char* m_buffer;
        int m_bufferSize;
        int m_topLine;
        int m_selected;
        int m_fileSize;

		enum EditMode
		{
			EditMode_None,
			EditMode_Byte,
			EditMode_Char
		};

		EditMode m_editMode;
		int m_nibbleIndex;
};

inline int HexView::GetSelectedLine()
{
    return m_selected >> 4;
}

inline int HexView::GetLastLine()
{
    return m_fileSize >> 4;
}

inline int HexView::GetBottomLine()
{
    return m_topLine + m_height - 1;
}

inline int HexView::GetSelectedOffset()
{
    return m_selected;
}

inline int HexView::GetFileSize()
{
    return m_fileSize;
}
