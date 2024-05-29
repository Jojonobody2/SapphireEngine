#pragma once

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
        virtual void OnDetach() {};
    };
}