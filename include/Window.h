#pragma once

#include "ConsoleBuffer.h"
#include "KeyEvent.h"
#include <vector>
#include <assert.h>

class Window
{
    public:
		Window();
        Window(Window* parent);

        virtual void OnWindowRefreshed();
		virtual void OnWindowResized(int width, int height);
        virtual void OnKeyEvent(KeyEvent& keyEvent);

        void AddChild(Window* window);

        void SetVisible(bool visible);
        bool IsVisible() const;
        void SetFocus(bool focus);
        bool HasFocus() const;

        static void SetConsoleBuffer(ConsoleBuffer* buffer);
        static void Add(Window* window);
        static void Resize(int width, int height);
        static void Refresh(bool fullDraw);
        static void ProcessKeyInput(KeyEvent& keyEvent);

    protected:
        int m_width;
        int m_height;
        Window* m_parent;

        static ConsoleBuffer* s_consoleBuffer;

    private:
        enum Flags
        {
           Flags_Focus = 1 << 0,
           Flags_Visible = 1 << 1
        };

        std::vector<Window*> m_children;
        unsigned int m_flags;

        static std::vector<Window*> s_rootWindows;
};
