#pragma once

class File
{
    public:
        File();

        bool Open(const char* path);
        void Close();
        int GetSize() const;
        void Seek(int position);
        void Read(void* buffer, int size);
        bool IsOpen() const;

    private:
        const char* m_path;
        int m_filesize;
        void* m_fp;
};

inline int File::GetSize() const
{
    return m_filesize;
}

inline bool File::IsOpen() const
{
    return m_fp != 0;
}
