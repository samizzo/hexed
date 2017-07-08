#include "File.h"
#include <Windows.h>

File::File()
{
    m_path = 0;
    m_filesize = 0;
    m_handle = 0;
}

bool File::Open(const char* path)
{
    m_path = path;

    m_handle = CreateFile(path, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_handle)
        m_filesize = GetFileSize(m_handle, 0);

    return m_handle != 0;
}

void File::Close()
{
    if (!m_handle)
        return;

    CloseHandle(m_handle);
}

void File::Seek(unsigned int position)
{
    if (!m_handle)
        return;

    LARGE_INTEGER distance;
    distance.QuadPart = position;
    SetFilePointerEx(m_handle, distance, 0, FILE_BEGIN);
}

void File::Read(void* buffer, unsigned int size)
{
    if (!m_handle)
        return;

    DWORD bytesRead;
    ReadFile(m_handle, buffer, size, &bytesRead, 0);
}
