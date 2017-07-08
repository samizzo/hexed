#include "File.h"
#include <stdio.h>

File::File()
{
    m_path = 0;
    m_filesize = 0;
    m_fp = 0;
}

bool File::Open(const char* path)
{
    m_path = path;

    FILE* fp = 0;
    fopen_s(&fp, m_path, "rb");
    m_fp = fp;
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        m_filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }

    return fp != 0;
}

void File::Close()
{
    if (m_fp)
        fclose((FILE*)m_fp);
}

void File::Seek(int position)
{
    fseek((FILE*)m_fp, position, SEEK_SET);
}

void File::Read(void* buffer, int size)
{
    fread_s(buffer, size, 1, size, (FILE*)m_fp);
}
