#pragma once

class KeyEvent
{
    public:
        KeyEvent(bool keyDown, unsigned short vkKeyCode, unsigned long controlKeyState);

        bool IsKeyDown() const;
        unsigned short GetVKKeyCode() const;
        bool IsControlKeyDown(int controlKey) const;

    private:
        bool m_keyDown;
        unsigned short m_vkKeyCode;
        unsigned long m_controlKeyState;
};

inline KeyEvent::KeyEvent(bool keyDown, unsigned short vkKeyCode, unsigned long controlKeyState)
{
    m_keyDown = keyDown;
    m_vkKeyCode = vkKeyCode;
    m_controlKeyState = controlKeyState;
}

inline bool KeyEvent::IsKeyDown() const
{
    return m_keyDown;
}

inline unsigned short KeyEvent::GetVKKeyCode() const
{
    return m_vkKeyCode;
}

inline bool KeyEvent::IsControlKeyDown(int controlKey) const
{
    return (m_controlKeyState & controlKey) != 0;
}
