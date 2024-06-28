#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire
{
    class VulkanRenderPlatform
    {
    public:
        static std::vector<const char*> GetPlatformInstanceExtensions();
        static VkSurfaceKHR CreateSurface(VkInstance Instance);
    };
}