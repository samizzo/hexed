#pragma once

#include <Windows.h>

class KeyEvent
{
    public:
        KeyEvent(KEY_EVENT_RECORD ker);

        const KEY_EVENT_RECORD& GetEvent();

    private:
        KEY_EVENT_RECORD m_ker;
};

inline KeyEvent::KeyEvent(KEY_EVENT_RECORD ker)
{
    m_ker = ker;
}

inline const KEY_EVENT_RECORD& KeyEvent::GetEvent()
{
    return m_ker;
}
