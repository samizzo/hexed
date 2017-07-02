#pragma once

#include "Window.h"

class HexView : public Window
{
    public:
        virtual void Draw();
        virtual void OnWindowResize(int newWidth, int newHeight);
};
