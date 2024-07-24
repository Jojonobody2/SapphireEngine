#pragma once

#include "GPUResources.h"

#include "Sapphire/Logging/Logger.h"

#include <vulkan/vulkan.h>

namespace Sapphire
{
    void VkCheck(VkResult Result);

    bool CheckInstanceLayerSupport(const std::vector<const char*>& Layers);
    bool CheckInstanceExtensionsSupport(const std::vector<const char*>& Extensions);

    bool CheckDeviceLayerSupport(VkPhysicalDevice PhysicalDevice, const std::vector<const char*>& Layers);
    bool CheckDeviceExtensionsSupport(VkPhysicalDevice PhysicalDevice, const std::vector<const char*>& Extensions);

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily{};
        std::optional<uint32_t> PresentFamily{};
    };

    QueueFamilyIndices SelectQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);

    void TransitionImageLayout(VkCommandBuffer Cmd, VkImage Image, VkImageLayout Old, VkImageLayout New, 
        VkImageSubresourceRange ImageSubresourceRange);
    void BlitImage(VkCommandBuffer Cmd, const GPUImage& Src, const GPUImage& Dst);
}