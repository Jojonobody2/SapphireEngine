#include "Renderer.h"
#include "Sapphire/Logging/Logger.h"

namespace Sapphire
{
    Renderer::Renderer()
    {
        SAPPHIRE_ENGINE_INFO("Initializing Rendering Engine!");

        m_RenderContext = IRenderContext::Create();
    }

    Renderer::~Renderer()
    {
        SAPPHIRE_ENGINE_INFO("Shutting Rendering Engine down!");
    }

    void Renderer::Draw()
    {

    }
}