#pragma once

#include <vulkan/vulkan.h>

namespace Sapphire
{
    struct Queue
    {
        VkQueue  DeviceQueue;
        uint32_t Family;
    };

    class RenderContext
    {
    public:
        RenderContext();
        virtual ~RenderContext();

        VkInstance GetInstance() { return m_Instance; }
        VkSurfaceKHR GetSurface() { return m_Surface; }
        VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
        VkDevice GetDevice() { return m_Device; }

        Queue GetGraphicsQueue() { return m_GraphicsQueue; }
        Queue GetPresentQueue() { return m_PresentQueue; }

    private:
        VkInstance m_Instance{};
        VkDebugUtilsMessengerEXT m_DebugMessenger{};
        VkSurfaceKHR m_Surface{};
        VkPhysicalDevice m_PhysicalDevice{};
        VkDevice m_Device{};

        Queue m_GraphicsQueue{};
        Queue m_PresentQueue{};

    private:
        void CreateInstance();
        void CreateDevice();
    };
}