#include <iostream>
#include "VulkanUtil.h"

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
                SAPPHIRE_ENGINE_ERROR("Vulkan Instance Extension: {} not available!", Layer);
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
}