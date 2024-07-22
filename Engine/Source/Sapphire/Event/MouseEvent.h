#pragma once

#include "SapphireEvent.h"

#include "Sapphire/Core/InputCodes.h"

namespace Sapphire
{
    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(const float x, const float y)
                : m_MouseX(x), m_MouseY(y) {}

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        EVENT_CLASS_TYPE(EventType::MouseMoved)

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(const float xOffset, const float yOffset)
                : m_xOffset(xOffset), m_yOffset(yOffset) {}

        float GetScrollX() const { return m_xOffset; }
        float GetScrollY() const { return m_yOffset; }

        EVENT_CLASS_TYPE(EventType::MouseScrolled)

    private:
        float m_xOffset, m_yOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseButton() const { return m_MouseButton; }

    protected:
        MouseButtonEvent(const MouseCode MouseButton)
                : m_MouseButton(MouseButton) {}

        MouseCode m_MouseButton;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(const MouseCode MouseButton)
                : MouseButtonEvent(MouseButton) {}

        EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(const MouseCode MouseButton)
                : MouseButtonEvent(MouseButton) {}

        EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
    };
}