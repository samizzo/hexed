#pragma once

class File
{
    public:
        File();

        bool Open(const char* path);
        void Close();
        unsigned int GetSize() const;
        void Seek(unsigned int position);
        void Read(void* buffer, unsigned int size);
        bool IsOpen() const;

    private:
        const char* m_path;
        unsigned int m_filesize;
        void* m_handle;
};

inline unsigned int File::GetSize() const
{
    return m_filesize;
}

inline bool File::IsOpen() const
{
    return m_handle != 0;
}
