#include "VulkanRenderContext.h"

#include "VulkanRenderExtensions.h"
#include "VulkanRenderPlatform.h"
#include "VulkanUtil.h"

#include "Sapphire/Core/Application.h"

namespace Sapphire
{
    VulkanRenderContext::VulkanRenderContext()
    {
        CreateInstance();
        CreateDevice();
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        #ifdef SAPPHIRE_RENDER_DEBUG
        EXT::vkDestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger, nullptr);
        #endif
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
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

        std::vector<const char*> InstanceLayers{};
        #ifdef SAPPHIRE_RENDER_DEBUG
        InstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        #endif

        if (!CheckInstanceLayerSupport(InstanceLayers))
        {
            SAPPHIRE_ENGINE_ERROR("Required Vulkan layer not available!");
            return;
        }

        //returns OS required extensions for Vulkan surface
        auto InstanceExtensions = VulkanRenderExtensions::GetRequiredInstanceExtensions();
        #ifdef SAPPHIRE_RENDER_DEBUG
        InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        if (!CheckInstanceExtensionsSupport(InstanceExtensions))
        {
            SAPPHIRE_ENGINE_ERROR("Required Vulkan extension not available!");
            return;
        }

        VkInstanceCreateInfo InstanceCreateInfo{};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pNext = nullptr;
        InstanceCreateInfo.flags = 0;
        InstanceCreateInfo.pApplicationInfo = &AppInfo;
        InstanceCreateInfo.enabledLayerCount = (uint32_t)InstanceLayers.size();
        InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.data();
        InstanceCreateInfo.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
        InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

        VkCheck(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance));
        VulkanRenderExtensions::LoadInstance(m_Instance);

        #ifdef SAPPHIRE_RENDER_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo{};
        DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfo.pfnUserCallback = VulkanRenderExtensions::DebugCallback;
        DebugUtilsMessengerCreateInfo.pUserData = nullptr;

        VkCheck(EXT::vkCreateDebugUtilsMessenger(m_Instance, &DebugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger));
        #endif

        m_Surface = VulkanRenderPlatform::CreateSurface(m_Instance);
    }

    void VulkanRenderContext::CreateDevice()
    {
        uint32_t DeviceCount{};
        VkCheck(vkEnumeratePhysicalDevices(m_Instance, &DeviceCount, nullptr));

        std::vector<VkPhysicalDevice> PhysicalDevices(DeviceCount);
        VkCheck(vkEnumeratePhysicalDevices(m_Instance, &DeviceCount, PhysicalDevices.data()));

        for (const auto& PhysicalDevice : PhysicalDevices)
        {
            VkPhysicalDeviceProperties DeviceProperties{};
            vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);

            if (DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_PhysicalDevice == PhysicalDevice;
                break;
            }
        }

        if (!m_PhysicalDevice && DeviceCount > 0)
        {
            m_PhysicalDevice = PhysicalDevices[0];
        }

        VkPhysicalDeviceProperties DeviceProperties{};
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &DeviceProperties);

        SAPPHIRE_ENGINE_INFO("Using GPU: {}", DeviceProperties.deviceName);

        QueueFamilyIndices QueueFamilies = SelectQueueFamilies(m_PhysicalDevice, m_Surface);
    }
}
