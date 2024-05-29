#include "IRenderContext.h"

#include "Sapphire/Platform/VulkanRHI/VulkanRenderContext.h"

namespace Sapphire
{
    ScopedPtr<Sapphire::IRenderContext> IRenderContext::Create()
    {
        return CreateScopedPtr<VulkanRenderContext>();
    }
}