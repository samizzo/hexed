#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <Windows.h>

void Log(const char* prefix, const char* format, ...)
{
    va_list args;
    OutputDebugString(prefix);
    va_start(args, format);
    int count = _vscprintf(format, args);
    char* buffer = (char*)_alloca(count + 1);
    vsprintf_s(buffer, count + 1, format, args);
    OutputDebugString(buffer);
    va_end(args);
}
