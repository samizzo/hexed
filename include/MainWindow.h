#pragma once

#include "Window.h"
#include "HexView.h"

class MainWindow : public Window
{
    public:
        MainWindow(const char* filename);
        virtual void OnWindowRefreshed();

    private:
        char m_fullPath[MAX_PATH];
        char* m_filename;
        HexView m_hexView;
};
