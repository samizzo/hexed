#pragma once

#include "Window.h"

class HexView : public Window
{
    public:
        HexView(ConsoleBuffer* consoleBuffer);
        virtual void Draw();
        virtual void OnWindowResize(int newWidth, int newHeight);
};
