#include "VulkanRenderContext.h"

#include "VulkanUtil.h"

#include "Sapphire/Core/Application.h"

namespace Sapphire
{
    VulkanRenderContext::VulkanRenderContext()
    {
        CreateInstance();
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        vkDestroyInstance(m_Instance, nullptr);
    }

    void VulkanRenderContext::CreateInstance()
    {
        VkApplicationInfo AppInfo{};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pNext = nullptr;
        AppInfo.pApplicationName = Application::Get().GetName().c_str();
        AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "Sapphire Engine";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

        VkInstanceCreateInfo InstanceCreateInfo{};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pNext = nullptr;
        InstanceCreateInfo.flags = 0;
        InstanceCreateInfo.pApplicationInfo = &AppInfo;
        InstanceCreateInfo.enabledLayerCount = 0;
        InstanceCreateInfo.ppEnabledLayerNames = nullptr;
        InstanceCreateInfo.enabledExtensionCount = 0;
        InstanceCreateInfo.ppEnabledExtensionNames = nullptr;

        VkCheck(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance));
    }
}
