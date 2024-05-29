#pragma once

#include "Sapphire/RHI/IRenderContext.h"

namespace Sapphire
{
    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        void Draw();

    private:
        ScopedPtr<IRenderContext> m_RenderContext{};
    };
}