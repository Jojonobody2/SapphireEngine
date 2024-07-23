#include <iostream>
#include "VulkanUtil.h"
#include "VulkanStructs.h"

namespace Sapphire
{
    void VkCheck(VkResult Result)
    {
    #ifdef SAPPHIRE_RENDER_DEBUG
        if (Result != VK_SUCCESS)
        {
            SAPPHIRE_ENGINE_ERROR("Vulkan retured result: {}", (int)Result);
        }
    #endif
    }

    bool CheckInstanceLayerSupport(const std::vector<const char*>& Layers)
    {
        uint32_t LayerCount{};
        vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

        std::vector<VkLayerProperties> InstanceLayers(LayerCount);
        vkEnumerateInstanceLayerProperties(&LayerCount, InstanceLayers.data());

        for (const auto& Layer : Layers)
        {
            bool Found = false;

            for (VkLayerProperties LayerProperties : InstanceLayers)
            {
                if (strcmp(Layer, LayerProperties.layerName) == 0)
                {
                    Found = true;
                    break;
                }
            }

            if (!Found)
            {
                SAPPHIRE_ENGINE_ERROR("Vulkan Instance Layer: {} not available!", Layer);
                return false;
            }
        }

        return true;
    }

    bool CheckInstanceExtensionsSupport(const std::vector<const char*>& Extensions)
    {
        uint32_t ExtensionsCount{};
        vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionsCount, nullptr);

        std::vector<VkExtensionProperties> InstanceExtensions(ExtensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionsCount, InstanceExtensions.data());

        for (const auto& Extension : Extensions)
        {
            bool Found = false;

            for (VkExtensionProperties ExtensionProperties : InstanceExtensions)
            {
                if (strcmp(Extension, ExtensionProperties.extensionName) == 0)
                {
                    Found = true;
                    break;
                }
            }

            if (!Found)
            {
                SAPPHIRE_ENGINE_ERROR("Vulkan Instance Extension: {} not available!", Extension);
                return false;
            }
        }

        return true;
    }

    bool CheckDeviceLayerSupport(VkPhysicalDevice PhysicalDevice, const std::vector<const char*>& Layers)
    {
        uint32_t LayerCount{};
        vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, nullptr);

        std::vector<VkLayerProperties> DeviceLayers(LayerCount);
        vkEnumerateDeviceLayerProperties(PhysicalDevice, &LayerCount, DeviceLayers.data());

        for (const auto& Layer : Layers)
        {
            bool Found = false;

            for (VkLayerProperties LayerProperties : DeviceLayers)
            {
                if (strcmp(Layer, LayerProperties.layerName) == 0)
                {
                    Found = true;
                    break;
                }
            }

            if (!Found)
            {
                SAPPHIRE_ENGINE_ERROR("Vulkan Device Layer: {} not available!", Layer);
                return false;
            }
        }

        return true;
    }

    bool CheckDeviceExtensionsSupport(VkPhysicalDevice PhysicalDevice, const std::vector<const char*>& Extensions)
    {
        uint32_t ExtensionsCount{};
        vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionsCount, nullptr);

        std::vector<VkExtensionProperties> DeviceExtensions(ExtensionsCount);
        vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionsCount, DeviceExtensions.data());

        for (const auto& Extension : Extensions)
        {
            bool Found = false;

            for (const auto& ExtensionProperties : DeviceExtensions)
            {
                if (strcmp(Extension, ExtensionProperties.extensionName) == 0)
                {
                    Found = true;
                    break;
                }
            }

            if (!Found)
            {
                SAPPHIRE_ENGINE_ERROR("Vulkan Instance Extension: {} not available!", Extension);
                return false;
            }
        }

        return true;
    }

    QueueFamilyIndices SelectQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
    {
        QueueFamilyIndices Indices{};

        uint32_t QueueFamilyCount{};
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies.data());

        int32_t Index{};
        for (const auto& QueueFamily : QueueFamilies)
        {
            if (QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                Indices.GraphicsFamily = Index;
            }

            VkBool32 PresentSupported{};
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &PresentSupported);

            if (PresentSupported)
            {
                Indices.PresentFamily = Index;
            }

            Index++;
        }

        return Indices;
    }

    void TransitionImageLayout(VkCommandBuffer Cmd, VkImage Image, VkImageLayout Old, VkImageLayout New, VkImageAspectFlags ImageAspect)
    {
        VkImageMemoryBarrier2 ImageBarrier{};
        ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        ImageBarrier.pNext = nullptr;
        ImageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        ImageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        ImageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        ImageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        ImageBarrier.oldLayout = Old;
        ImageBarrier.newLayout = New;
        ImageBarrier.subresourceRange = ImageSubresourceRange(ImageAspect);
        ImageBarrier.image = Image;

        VkDependencyInfo DependencyInfo{};
        DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        DependencyInfo.pNext = nullptr;
        DependencyInfo.imageMemoryBarrierCount = 1;
        DependencyInfo.pImageMemoryBarriers = &ImageBarrier;

        vkCmdPipelineBarrier2(Cmd, &DependencyInfo);
    }

    void BlitImage(VkCommandBuffer Cmd, const GPUImage& Src, const GPUImage& Dst)
    {
        VkImageBlit2 ImageBlitRegion{};
        ImageBlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
        ImageBlitRegion.pNext = nullptr;
        ImageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageBlitRegion.srcSubresource.baseArrayLayer = 0;
        ImageBlitRegion.srcSubresource.layerCount = 1;
        ImageBlitRegion.dstSubresource.mipLevel = 0;
        ImageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageBlitRegion.dstSubresource.baseArrayLayer = 0;
        ImageBlitRegion.dstSubresource.layerCount = 1;
        ImageBlitRegion.dstSubresource.mipLevel = 0;
        ImageBlitRegion.srcOffsets[1].x = Src.ImageSize.width;
        ImageBlitRegion.srcOffsets[1].y = Src.ImageSize.height;
        ImageBlitRegion.srcOffsets[1].z = 1;
        ImageBlitRegion.dstOffsets[1].x = Dst.ImageSize.width;
        ImageBlitRegion.dstOffsets[1].y = Dst.ImageSize.height;
        ImageBlitRegion.dstOffsets[1].z = 1;

        VkBlitImageInfo2 BlitImageInfo{};
        BlitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
        BlitImageInfo.pNext = nullptr;
        BlitImageInfo.srcImage = Src.Image;
        BlitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        BlitImageInfo.dstImage = Dst.Image;
        BlitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        BlitImageInfo.regionCount = 1;
        BlitImageInfo.pRegions = &ImageBlitRegion;

        vkCmdBlitImage2(Cmd, &BlitImageInfo);
    }
}