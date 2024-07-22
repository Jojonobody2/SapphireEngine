#pragma once

#include "SapphireEvent.h"

#include "Sapphire/Core/InputCodes.h"

namespace Sapphire
{
    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(const KeyCode KeyCode)
                : m_KeyCode(KeyCode) {}

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(const KeyCode KeyCode)
                : KeyEvent(KeyCode) {}

        EVENT_CLASS_TYPE(EventType::KeyPressed)
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(const KeyCode KeyCode)
                : KeyEvent(KeyCode) {}

        EVENT_CLASS_TYPE(EventType::KeyReleased)
    };

    class KeyTypedEvent : public Event
    {
    public:
        KeyTypedEvent(const char Char)
                : m_Char(Char) {}

        char GetTypedChar() const { return m_Char; }

        EVENT_CLASS_TYPE(EventType::KeyTyped)

    private:
        char m_Char;
    };
}