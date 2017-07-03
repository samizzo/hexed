#pragma once

#include "Window.h"
#include <stdio.h>

class HexView : public Window
{
    public:
        HexView(ConsoleBuffer* consoleBuffer, const char* filename);
        ~HexView();

        virtual void OnWindowRefreshed();
        virtual void OnWindowResized(int newWidth, int newHeight);

    private:
        void CacheFile();

        FILE* m_fp;
        unsigned char* m_buffer;
        int m_bufferSize;
        int m_offset;
        int m_fileSize;
};
