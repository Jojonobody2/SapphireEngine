#pragma once

#include <string>
#include <iostream>

namespace Sapphire
{
    class ILayer
    {
    public:
        virtual void OnAttach() {};
        virtual void OnUpdate() {};
        virtual void OnDetach() {};
    };
}