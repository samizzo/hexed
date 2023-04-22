#pragma once

void Log(const char* prefix, const char* format, ...);

#if _DEBUG
#define LogDebug(...) Log("DEBUG: ", __VA_ARGS__)
#else
#define LogDebug(...)
#endif
