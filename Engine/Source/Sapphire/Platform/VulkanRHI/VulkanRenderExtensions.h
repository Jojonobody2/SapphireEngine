#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire
{
    class VulkanRenderExtensions
    {
    public:
        static std::vector<const char*> GetRequiredInstanceExtensions();
        static void LoadInstance(VkInstance Instance);


        #ifdef SAPPHIRE_RENDER_DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT MessageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* PCallbackData,
                                                            void* PUserData);
        #endif
    };

    namespace EXT
    {
        extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger;
        extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger;
    }
}