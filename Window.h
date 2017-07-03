#pragma once

#include <Windows.h>
#include "ConsoleBuffer.h"

class Window
{
    public:
		Window(ConsoleBuffer* consoleBuffer, const char* filename);

        virtual void Draw();
		virtual void OnWindowResize(int width, int height);

    protected:
        const char* m_filename;
        int m_width;
        int m_height;
        ConsoleBuffer* m_consoleBuffer;
};

