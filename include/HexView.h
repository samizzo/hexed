#pragma once

#include "Window.h"
#include <stdio.h>

class HexView : public Window
{
    public:
        HexView(const char* filename);
        ~HexView();

        virtual void OnWindowRefreshed();
        virtual void OnWindowResized(int newWidth, int newHeight);
        virtual void OnKeyEvent(const KEY_EVENT_RECORD& ker);

    private:
        void CacheFile(bool resizeBuffer = false);

        int GetSelectedLine();
        int GetLastLine();
        int GetBottomLine();

        FILE* m_fp;
        unsigned char* m_buffer;
        int m_bufferSize;
        int m_topLine;
        int m_selected;
        int m_fileSize;
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
