#include <vulkan/vulkan.h>
#include "Sapphire/Logging/Logger.h"

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
}