#ifdef SAPPHIRE_LINUX

#include "../VulkanRenderPlatform.h"
#include "../VulkanUtil.h"

#include "Sapphire/Core/Application.h"

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

    VkSurfaceKHR VulkanRenderPlatform::CreateSurface(VkInstance Instance)
    {
        VkSurfaceKHR Surface;
        VkCheck(glfwCreateWindowSurface(Instance, static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), nullptr, &Surface));

        return Surface;
    }
}

#endif