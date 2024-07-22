#pragma once

#include "RenderContext.h"
#include "Sapphire/Core/Base.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire
{
    class CommandList;

    class Swapchain
    {
    public:
        Swapchain(const SharedPtr<RenderContext>& RenderContext, uint32_t FramesInFlight);
        virtual ~Swapchain();

        uint32_t AcquireNextImage(uint32_t FrameIndex);
        void Present(const SharedPtr<CommandList>& CommandList);

        VkImage GetImage(uint32_t Index);
        VkImageView GetImageView(uint32_t Index);

        VkExtent2D GetExtent() { return m_Extent; }
        VkSurfaceFormatKHR GetFormat() { return m_SurfaceFormat; }
        [[nodiscard]] uint32_t GetImageCount() const { return m_ImageCount; }
        VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

        VkSemaphore GetSemaphore(uint32_t FrameIndex);

    private:
        VkExtent2D m_Extent{};
        VkSurfaceFormatKHR m_SurfaceFormat{};
        uint32_t m_ImageCount{};

        VkSwapchainKHR m_Swapchain{};
        std::vector<VkImage> m_Images{};
        std::vector<VkImageView> m_ImageViews{};

        std::vector<VkSemaphore> m_PresentSemaphores{};

        //needs to be cached for presentation
        uint32_t m_CurrentImageIndex{};

    private:
        //store VkDevice for destruction
        SharedPtr<RenderContext> m_RenderContext;
    };
}
