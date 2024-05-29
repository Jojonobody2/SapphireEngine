#pragma once

#include "Sapphire/Event/SapphireEvent.h"

#include <string>
#include <iostream>

namespace Sapphire
{
    class ILayer
    {
    public:
        virtual ~ILayer() = default;
        virtual void OnAttach() {};
        virtual void OnUpdate() {};
        virtual void OnEvent(Event& Event) {};
        virtual void OnDetach() {};
    };
}