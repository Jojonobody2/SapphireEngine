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
        VkDebugUtilsMessengerEXT m_DebugMessenger{};
        VkSurfaceKHR m_Surface{};
        VkPhysicalDevice m_PhysicalDevice{};

    private:
        void CreateInstance();
        void CreateDevice();
    };
}