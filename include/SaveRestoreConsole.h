#pragma once

#include <Windows.h>

void SaveConsole(HANDLE stdoutHandle, HANDLE stdinHandle);
void RestoreConsole(HANDLE stdoutHandle, HANDLE stdinHandle);
