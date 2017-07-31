#pragma once

#include "Window.h"
#include "HexView.h"
#include "HelpWindow.h"
#include "File.h"

class MainWindow : public Window
{
    public:
        MainWindow(File* file);
        virtual void OnWindowRefreshed();
        virtual void OnKeyEvent(KeyEvent& keyEvent);

    private:
        char m_fullPath[MAX_PATH];
        HexView m_hexView;
        HelpWindow m_helpWindow;
		File* m_file;
};
