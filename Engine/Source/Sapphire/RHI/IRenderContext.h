#pragma once

#include "Sapphire/Core/Base.h"

namespace Sapphire
{
    class IRenderContext
    {
    public:
        virtual ~IRenderContext() = default;

        static ScopedPtr<IRenderContext> Create();
    };
}