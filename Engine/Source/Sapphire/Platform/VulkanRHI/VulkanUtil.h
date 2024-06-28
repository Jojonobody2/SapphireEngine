#include "Sapphire/Logging/Logger.h"

#include <vulkan/vulkan.h>

namespace Sapphire
{
    void VkCheck(VkResult Result);

    bool CheckInstanceLayerSupport(const std::vector<const char*>& Layers);
    bool CheckInstanceExtensionsSupport(const std::vector<const char*>& Extensions);

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily{};
        std::optional<uint32_t> PresentFamily{};
    };

    QueueFamilyIndices SelectQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
}