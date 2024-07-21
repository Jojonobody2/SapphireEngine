#ifdef SAPPHIRE_LINUX

#include "Sapphire/Renderer/RenderPlatform.h"
#include "Sapphire/Renderer/VulkanUtil.h"

#include "Sapphire/Core/Application.h"

#include "GLFW/glfw3.h"

namespace Sapphire
{
    std::vector<const char*> RenderPlatform::GetPlatformInstanceExtensions()
    {
        uint32_t Count;
        const char** CExtensions = glfwGetRequiredInstanceExtensions(&Count);

        std::vector<const char*> Extensions(CExtensions, CExtensions + Count);

        return Extensions;
    }

    VkSurfaceKHR RenderPlatform::CreateSurface(VkInstance Instance)
    {
        VkSurfaceKHR Surface;
        VkCheck(glfwCreateWindowSurface(Instance, static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), nullptr, &Surface));

        return Surface;
    }
}

#endif