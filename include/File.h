#pragma once

class File
{
    public:
        File();
        ~File();

        bool Open(const char* path);
        void Close();
        unsigned int GetSize() const;
        void Seek(unsigned int position);
        void Read(void* buffer, unsigned int size);
        void Write(void* buffer, unsigned int size);
        bool IsOpen() const;
        bool IsReadOnly() const;
        const char* GetFullPath() const;
        const char* GetFileName() const;

    private:
        char* m_filename;
        char* m_fullPath;
        unsigned int m_filesize;
        void* m_handle;
        bool m_readOnly;
};

inline unsigned int File::GetSize() const
{
    return m_filesize;
}

inline bool File::IsReadOnly() const
{
    return m_readOnly;
}

inline const char* File::GetFullPath() const
{
    return m_fullPath;
}

inline const char* File::GetFileName() const
{
    return m_filename;
}
