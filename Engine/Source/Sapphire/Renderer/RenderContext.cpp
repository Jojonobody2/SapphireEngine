#include "RenderContext.h"

#include "RenderExtensions.h"
#include "RenderPlatform.h"
#include "VulkanUtil.h"

#include "Sapphire/Core/Application.h"

#include <set>

namespace Sapphire
{
    RenderContext::RenderContext()
    {
        CreateInstance();
        CreateDevice();
    }

    RenderContext::~RenderContext()
    {
        vkDestroyDevice(m_Device, nullptr);

        #ifdef SAPPHIRE_RENDER_DEBUG
        EXT::vkDestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger, nullptr);
        #endif
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void RenderContext::CreateInstance()
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
        auto InstanceExtensions = RenderExtensions::GetRequiredInstanceExtensions();
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
        RenderExtensions::LoadInstance(m_Instance);

        #ifdef SAPPHIRE_RENDER_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo{};
        DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfo.pfnUserCallback = RenderExtensions::DebugCallback;
        DebugUtilsMessengerCreateInfo.pUserData = nullptr;

        VkCheck(EXT::vkCreateDebugUtilsMessenger(m_Instance, &DebugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger));
        #endif

        m_Surface = RenderPlatform::CreateSurface(m_Instance);
    }

    void RenderContext::CreateDevice()
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
                m_PhysicalDevice = PhysicalDevice;
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

        std::set<uint32_t> UniqueQueueFamilies = { QueueFamilies.GraphicsFamily.value(), QueueFamilies.PresentFamily.value() };

        std::vector<VkDeviceQueueCreateInfo> DeviceQueueCreateInfos{};
        float QueuePriority = 1.f;

        for (uint32_t QueueFamily : UniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo QueueCreateInfo{};
            QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            QueueCreateInfo.queueFamilyIndex = QueueFamily;
            QueueCreateInfo.queueCount = 1;
            QueueCreateInfo.pQueuePriorities = &QueuePriority;

            DeviceQueueCreateInfos.push_back(QueueCreateInfo);
        }

        VkPhysicalDeviceFeatures DeviceFeatures{};

        std::vector<const char*> DeviceLayers{};
        #ifdef SAPPHIRE_RENDER_DEBUG
        DeviceLayers.push_back("VK_LAYER_KHRONOS_validation");
        #endif

        if (!CheckDeviceLayerSupport(m_PhysicalDevice, DeviceLayers))
        {
            SAPPHIRE_ENGINE_ERROR("Required Vulkan layer not available!");
            return;
        }

        std::vector<const char*> DeviceExtensions{};
        DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (!CheckDeviceExtensionsSupport(m_PhysicalDevice, DeviceExtensions))
        {
            SAPPHIRE_ENGINE_ERROR("Required Vulkan extension not available!");
            return;
        }

        VkPhysicalDeviceVulkan12Features Vulkan12Features{};
        Vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        Vulkan12Features.bufferDeviceAddress = true;

        VkPhysicalDeviceVulkan13Features Vulkan13Features{};
        Vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        Vulkan13Features.pNext = &Vulkan12Features;
        Vulkan13Features.synchronization2 = true;
        Vulkan13Features.dynamicRendering = true;

        VkDeviceCreateInfo DeviceCreateInfo{};
        DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        DeviceCreateInfo.pNext = &Vulkan13Features;
        DeviceCreateInfo.queueCreateInfoCount = (uint32_t)DeviceQueueCreateInfos.size();
        DeviceCreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos.data();
        DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
        DeviceCreateInfo.enabledExtensionCount = (uint32_t)DeviceExtensions.size();
        DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
        DeviceCreateInfo.enabledLayerCount = (uint32_t)DeviceLayers.size();
        DeviceCreateInfo.ppEnabledLayerNames = DeviceLayers.data();

        VkCheck(vkCreateDevice(m_PhysicalDevice, &DeviceCreateInfo, nullptr, &m_Device));

        m_GraphicsQueue.Family = QueueFamilies.GraphicsFamily.value();
        m_PresentQueue.Family = QueueFamilies.PresentFamily.value();

        vkGetDeviceQueue(m_Device, m_GraphicsQueue.Family, 0, &m_GraphicsQueue.DeviceQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueue.Family, 0, &m_PresentQueue.DeviceQueue);
    }
}
