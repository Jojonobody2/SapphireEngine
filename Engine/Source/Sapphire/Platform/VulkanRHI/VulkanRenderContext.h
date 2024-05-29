#pragma once

#include "Sapphire/RHI/IRenderContext.h"

#include <vulkan/vulkan.h>

namespace Sapphire
{
    class VulkanRenderContext : public IRenderContext
    {
    public:
        VulkanRenderContext();
        ~VulkanRenderContext() override;

    private:
        VkInstance m_Instance{};

    private:
        void CreateInstance();
    };
}