#include "Error.h"
#include <string>
#include <Windows.h>
#include <stdio.h>

using namespace std;

extern bool s_running;

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
