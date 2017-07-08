#pragma once

#include "Window.h"
#include "HexView.h"
#include "HelpWindow.h"

class MainWindow : public Window
{
    public:
        MainWindow(const char* filename);
        virtual void OnWindowRefreshed();
        virtual void OnKeyEvent(KeyEvent& keyEvent);

    private:
        char m_fullPath[MAX_PATH];
        char* m_filename;
        HexView m_hexView;
        HelpWindow m_helpWindow;
};
