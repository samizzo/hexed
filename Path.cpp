#include "Path.h"
#include <string.h>

const char* Path::FindFileName(const char* path)
{
    size_t length = strlen(path);
    const char* p = path + length - 1;
    for (size_t i = length - 1; i >= 0 && *p != '/' && *p != '\\'; i--, p--);
    return p + 1;
}
