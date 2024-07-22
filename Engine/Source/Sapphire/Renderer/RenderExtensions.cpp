#include "RenderExtensions.h"

#include "RenderPlatform.h"
#include "Sapphire/Logging/Logger.h"

namespace Sapphire
{
    void RenderExtensions::LoadInstance(VkInstance Instance)
    {
        EXT::vkCreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT"));
        EXT::vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT"));
    }

    std::vector<const char*> RenderExtensions::GetRequiredInstanceExtensions()
    {
        std::vector<const char*> InstanceExtensions{};

        const auto PlatformExtensions = RenderPlatform::GetPlatformInstanceExtensions();
        InstanceExtensions.insert(InstanceExtensions.begin(), PlatformExtensions.begin(), PlatformExtensions.end());

        #ifdef SAPPHIRE_RENDER_DEBUG
        InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        return InstanceExtensions;
    }

    #ifdef SAPPHIRE_RENDER_DEBUG
    VkBool32 RenderExtensions::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT MessageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* PCallbackData,
                                             void* PUserData)
    {
        const char* Message = PCallbackData->pMessage;

        switch (MessageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                SAPPHIRE_ENGINE_WARN("Validation Layer: {}", Message);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                SAPPHIRE_ENGINE_ERROR("Validation Layer: {}", Message);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            {
                SAPPHIRE_ENGINE_CRITICAL("Validation Layer: {}", Message);
                break;
            }
            default:
            {
                break;
            }
        }

        return VK_FALSE;
    }
    #endif

    namespace EXT
    {
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger;
    }
}