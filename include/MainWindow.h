#pragma once

#include "Window.h"

class MainWindow : public Window
{
    public:
        MainWindow(const char* filename);
        virtual void OnWindowRefreshed();

    private:
        const char* m_filename;
};
