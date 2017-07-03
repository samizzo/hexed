#include "Window.h"
#include "HexView.h"
#include <stdio.h>
#include <string>

using namespace std;

static DWORD s_prevMode;
static bool s_running = false;
static HANDLE s_stdoutHandle;
static HANDLE s_stdinHandle;
static CONSOLE_SCREEN_BUFFER_INFOEX s_prevInfo;

string FormatErrorMessage(DWORD ErrorCode)
{
    TCHAR* pMsgBuf = NULL;
    DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&pMsgBuf), 0, NULL);
    if (!nMsgLen)
        return "FormatMessage failed";
    string msg(pMsgBuf);
    LocalFree(pMsgBuf);
    return msg;
}

void Error(const char* message)
{
    printf("Error: %s\n", message);
    DWORD lastError = GetLastError();
    string errorString = FormatErrorMessage(lastError);
    printf("LastError: %s\n", errorString.c_str());
    if (!s_running)
        ExitProcess(1);
}

void ProcessInput(const INPUT_RECORD& inputRecord);
void ProcessKeyEvent(const KEY_EVENT_RECORD& ker);
void OnWindowResized(int newWidth, int newHeight);
void RemapColours();
void SaveConsole();
void RestoreConsole();

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: hexed <filename>\n");
        return 0;
    }

    s_stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (s_stdinHandle == INVALID_HANDLE_VALUE)
        Error("Couldn't get input handle");

    s_stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (s_stdoutHandle == INVALID_HANDLE_VALUE)
        Error("Couldn't get output handle");

    SaveConsole();

    if (!SetConsoleMode(s_stdinHandle, ENABLE_WINDOW_INPUT))
        Error("Couldn't set console mode");

    static const int INPUT_BUFFER_SIZE = 128;
    INPUT_RECORD inputBuffer[INPUT_BUFFER_SIZE];

    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(s_stdoutHandle, &info))
        Error("Couldn't get console screen buffer info");

    RemapColours();

    int width = info.srWindow.Right - info.srWindow.Left + 1;
    int height = info.srWindow.Bottom - info.srWindow.Top + 1;

    ConsoleBuffer buffer(s_stdoutHandle);
    Window window(&buffer);
    HexView hexView(&buffer);

    hexView.SetPosition(0, 1);

    bool windowResized = true;
    s_running = true;

    while (s_running)
    {
        DWORD numEventsAvailable;
        GetNumberOfConsoleInputEvents(s_stdinHandle, &numEventsAvailable);
        if (numEventsAvailable > 0)
        {
            DWORD numEventsRead;
            ReadConsoleInput(s_stdinHandle, inputBuffer, INPUT_BUFFER_SIZE, &numEventsRead);

            for (DWORD i = 0; i < numEventsRead; i++)
            {
                INPUT_RECORD& e = inputBuffer[i];
                ProcessInput(e);
            }
        }

        CONSOLE_SCREEN_BUFFER_INFO newInfo;
        if (GetConsoleScreenBufferInfo(s_stdoutHandle, &newInfo))
        {
            int newWidth = newInfo.srWindow.Right - newInfo.srWindow.Left + 1;
            int newHeight = newInfo.srWindow.Bottom - newInfo.srWindow.Top + 1;
            if (newWidth != width || newHeight != height)
            {
                info = newInfo;
                width = newWidth;
                height = newHeight;
                OnWindowResized(newWidth, newHeight);
                windowResized = true;
            }
        }

        if (windowResized)
        {
            // TODO: Clear entire buffer before drawing?
            // FarManager scrolls to the bottom before drawing.

            buffer.OnWindowResize(width, height);

            window.OnWindowResize(width, height);
            window.Draw();

            hexView.OnWindowResize(width, height);
            hexView.Draw();

            buffer.Flush();

            windowResized = false;
        }
    }

    RestoreConsole();
}

void ProcessInput(const INPUT_RECORD& inputRecord)
{
    switch (inputRecord.EventType)
    {
        case KEY_EVENT:
        {
            ProcessKeyEvent(inputRecord.Event.KeyEvent);
            break;
        }
    }
}

void ProcessKeyEvent(const KEY_EVENT_RECORD& ker)
{
    if (ker.wVirtualKeyCode == VK_ESCAPE && !ker.bKeyDown)
    {
        s_running = false;
    }
}

void OnWindowResized(int newWidth, int newHeight)
{
}

void RemapColours()
{
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(s_stdoutHandle, &info))
        Error("Failed to get console info");

    // TODO: Configurable colours.

    info.ColorTable[0] = 0x00362b00;
    info.ColorTable[1] = 0x00423607;
    info.ColorTable[2] = 0x00808000;
    info.ColorTable[3] = 0x00a48231;
    info.ColorTable[4] = 0x00164bcb;
    info.ColorTable[5] = 0x00b6369c;
    info.ColorTable[6] = 0x00009985;
    info.ColorTable[7] = 0x00d5e8ee;
    info.ColorTable[8] = 0x00a1a193;
    info.ColorTable[9] = 0x00d28b26;
    info.ColorTable[10] = 0x0036b64f;
    info.ColorTable[11] = 0x0098a12a;
    info.ColorTable[12] = 0x002f32dc;
    info.ColorTable[13] = 0x008236d3;
    info.ColorTable[14] = 0x000089b5;
    info.ColorTable[15] = 0x00e3f6fd;

    // HACK: Workaround for a bug where SetConsoleScreenBufferInfoEx seems to interpret
    // the right and bottom as exclusive, so the window size shrinks.
    info.srWindow.Right += 1;
    info.srWindow.Bottom += 1;

    if (!SetConsoleScreenBufferInfoEx(s_stdoutHandle, &info))
        Error("Failed to set console info");
}

void SaveConsole()
{
    // TODO: Save console buffer and restore it when exiting.
    s_prevInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    if (!GetConsoleScreenBufferInfoEx(s_stdoutHandle, &s_prevInfo))
        Error("Failed to get console info");

    if (!GetConsoleMode(s_stdinHandle, &s_prevMode))
        Error("Couldn't get console mode");
}

void RestoreConsole()
{
    s_prevInfo.srWindow.Right += 1;
    s_prevInfo.srWindow.Bottom += 1;
    SetConsoleScreenBufferInfoEx(s_stdoutHandle, &s_prevInfo);
    SetConsoleMode(s_stdinHandle, s_prevMode);
}
