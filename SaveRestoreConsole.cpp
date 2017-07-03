#include "SaveRestoreConsole.h"
#include "Error.h"

static CONSOLE_SCREEN_BUFFER_INFOEX s_prevInfo;
static DWORD s_prevMode;

void SaveConsole(HANDLE stdoutHandle, HANDLE stdinHandle)
{
    // TODO: Save console buffer and restore it when exiting.
    s_prevInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(stdoutHandle, &s_prevInfo))
        Error("Failed to get console info");

    if (!GetConsoleMode(stdinHandle, &s_prevMode))
        Error("Couldn't get console mode");
}

void RestoreConsole(HANDLE stdoutHandle, HANDLE stdinHandle)
{
    s_prevInfo.srWindow.Right += 1;
    s_prevInfo.srWindow.Bottom += 1;
    SetConsoleScreenBufferInfoEx(stdoutHandle, &s_prevInfo);
    SetConsoleMode(stdinHandle, s_prevMode);
}
