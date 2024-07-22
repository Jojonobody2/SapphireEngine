#pragma once

namespace Sapphire
{
    #define EVENT_CLASS_TYPE(Type) virtual EventType GetEventType() const override { return Type; }

    enum class EventType
    {
        None = 0,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
    };

    class Event
    {
    public:
        virtual ~Event() = default;

        [[nodiscard]] virtual EventType GetEventType() const = 0;
    };
}