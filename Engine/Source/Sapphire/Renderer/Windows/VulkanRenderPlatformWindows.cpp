#ifdef SAPPHIRE_WINDOWS

#include "../VulkanRenderPlatform.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
    std::vector<const char*> VulkanRenderPlatform::GetPlatformInstanceExtensions()
    {
        uint32_t Count;
        const char** CExtensions = glfwGetRequiredInstanceExtensions(&Count);

        std::vector<const char*> Extensions(CExtensions, CExtensions + Count);

        return Extensions;
    }
}

#endif