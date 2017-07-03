#include "SaveRestoreConsole.h"
#include "Error.h"
#include <stdio.h>
#include <assert.h>

static CONSOLE_SCREEN_BUFFER_INFOEX s_prevInfo;
static DWORD s_prevMode;
static CHAR_INFO* s_prevConsoleOutput;
static COORD s_prevSize;

void SaveConsole(HANDLE stdoutHandle, HANDLE stdinHandle)
{
    s_prevInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(stdoutHandle, &s_prevInfo))
        Error("Failed to get console info");

    if (!GetConsoleMode(stdinHandle, &s_prevMode))
        Error("Couldn't get console mode");

    // Resize the buffer to the same size as the window.
    s_prevSize.X = s_prevInfo.srWindow.Right - s_prevInfo.srWindow.Left + 1;
    s_prevSize.Y = s_prevInfo.srWindow.Bottom - s_prevInfo.srWindow.Top + 1;
    SetConsoleScreenBufferSize(stdoutHandle, s_prevSize);

    // Save the current output in the newly resized window.
    s_prevConsoleOutput = new CHAR_INFO[s_prevSize.X * s_prevSize.Y];

    COORD coord;
    coord.X = 0;
    coord.Y = 0;

    SMALL_RECT rect;
    rect.Left = 0;
    rect.Top = 0;
    rect.Right = s_prevSize.X;
    rect.Bottom = s_prevSize.Y;
    ReadConsoleOutput(stdoutHandle, s_prevConsoleOutput, s_prevSize, coord, &rect);
}

void RestoreConsole(HANDLE stdoutHandle, HANDLE stdinHandle)
{
    {
        // Restore the old output in the window.
        assert(s_prevConsoleOutput != 0);
        COORD coord;
        coord.X = 0;
        coord.Y = 0;

        SMALL_RECT rect;
        rect.Left = 0;
        rect.Right = s_prevSize.X;
        rect.Top = 0;
        rect.Bottom = s_prevSize.Y;
        WriteConsoleOutput(stdoutHandle, s_prevConsoleOutput, s_prevSize, coord, &rect);

        delete[] s_prevConsoleOutput;
        s_prevConsoleOutput = 0;
    }

    {
        SMALL_RECT& rect = s_prevInfo.srWindow;
        int width = rect.Right - rect.Left + 1;
        int height = rect.Bottom - rect.Top + 1;

        // HACK: Add 1 to work around a bug where the window shrinks otherwise.
        rect.Left = 0;
        rect.Right = width;
        rect.Top = 0;
        rect.Bottom = height;
        SetConsoleScreenBufferInfoEx(stdoutHandle, &s_prevInfo);
        SetConsoleMode(stdinHandle, s_prevMode);
    }
}
