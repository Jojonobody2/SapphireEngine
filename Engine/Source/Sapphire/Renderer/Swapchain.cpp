#include "Swapchain.h"

#include "VulkanUtil.h"
#include "VulkanStructs.h"

#include "Sapphire/Core/Application.h"
#include "CommandList.h"

namespace Sapphire
{
    Swapchain::Swapchain(const SharedPtr<RenderContext>& RenderContext, uint32_t FramesInFlight)
    {
        m_RenderContext = RenderContext;

        VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
        VkCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(RenderContext->GetPhysicalDevice(), RenderContext->GetSurface(), &SurfaceCapabilities));

        m_Extent = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };

        uint32_t FormatCount{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(RenderContext->GetPhysicalDevice(), RenderContext->GetSurface(), &FormatCount,
                                             nullptr);

        std::vector<VkSurfaceFormatKHR> SurfaceFormats(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(RenderContext->GetPhysicalDevice(), RenderContext->GetSurface(), &FormatCount,
                                             SurfaceFormats.data());

        for (const auto& Format : SurfaceFormats)
        {
            if (Format.format == VK_FORMAT_R8G8B8A8_UNORM && Format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = Format;
                break;
            }

            if (Format.format == VK_FORMAT_B8G8R8A8_UNORM && Format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = Format;
                break;
            }
        }

        uint32_t PresentModeCount{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(RenderContext->GetPhysicalDevice(), RenderContext->GetSurface(), &PresentModeCount,
                                             nullptr);

        std::vector<VkPresentModeKHR> PresentModes(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(RenderContext->GetPhysicalDevice(), RenderContext->GetSurface(), &PresentModeCount,
                                                  PresentModes.data());

        VkPresentModeKHR SelectedPresentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;

        for (const auto& PresentMode : PresentModes)
        {
            if (PresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR || PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                SelectedPresentMode = PresentMode;
                break;
            }
        }

        if (SelectedPresentMode == VK_PRESENT_MODE_MAX_ENUM_KHR)
        {
            //FIFO is required by Vulkan spec
            SelectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        m_ImageCount = SurfaceCapabilities.minImageCount + 1;

        VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
        SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        SwapchainCreateInfo.pNext = nullptr;
        SwapchainCreateInfo.flags = 0;
        SwapchainCreateInfo.surface = RenderContext->GetSurface();
        SwapchainCreateInfo.minImageCount = m_ImageCount;
        SwapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
        SwapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        SwapchainCreateInfo.imageExtent = m_Extent;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (RenderContext->GetGraphicsQueue().Family != RenderContext->GetPresentQueue().Family)
        {
            uint32_t QueueIndices[] = { RenderContext->GetGraphicsQueue().Family, RenderContext->GetPresentQueue().Family };

            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = 2;
            SwapchainCreateInfo.pQueueFamilyIndices = QueueIndices;
        }
        else
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            SwapchainCreateInfo.queueFamilyIndexCount = 0;
            SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }
        SwapchainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.presentMode = SelectedPresentMode;
        SwapchainCreateInfo.clipped = VK_TRUE;
        SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VkCheck(vkCreateSwapchainKHR(m_RenderContext->GetDevice(), &SwapchainCreateInfo, nullptr, &m_Swapchain));

        vkGetSwapchainImagesKHR(m_RenderContext->GetDevice(), m_Swapchain, &m_ImageCount, nullptr);
        m_Images.resize(m_ImageCount);
        vkGetSwapchainImagesKHR(m_RenderContext->GetDevice(), m_Swapchain, &m_ImageCount, m_Images.data());

        VkImageViewCreateInfo ImageViewCreateInfo = ImageViewInfo(m_Images[0], m_SurfaceFormat.format);

        m_ImageViews.resize(m_ImageCount);

        for (size_t i = 0; i < m_ImageCount; i++)
        {
            ImageViewCreateInfo.image = m_Images[i];
            VkCheck(vkCreateImageView(m_RenderContext->GetDevice(), &ImageViewCreateInfo, nullptr, &m_ImageViews[i]));
        }

        VkSemaphoreCreateInfo SemaphoreCreateInfo = SemaphoreInfo();

        m_PresentSemaphores.resize(FramesInFlight);
        for (uint32_t i = 0; i < FramesInFlight; i++)
        {
            VkCheck(vkCreateSemaphore(m_RenderContext->GetDevice(), &SemaphoreCreateInfo, nullptr, &m_PresentSemaphores[i]));
        }
    }

    Swapchain::~Swapchain()
    {
        for (VkSemaphore PresentSemaphore : m_PresentSemaphores)
        {
            vkDestroySemaphore(m_RenderContext->GetDevice(), PresentSemaphore, nullptr);
        }

        for (VkImageView ImageView : m_ImageViews)
        {
            vkDestroyImageView(m_RenderContext->GetDevice(), ImageView, nullptr);
        }

        vkDestroySwapchainKHR(m_RenderContext->GetDevice(), m_Swapchain, nullptr);
    }

    uint32_t Swapchain::AcquireNextImage(uint32_t FrameIndex)
    {
        VkCheck(vkAcquireNextImageKHR(m_RenderContext->GetDevice(), m_Swapchain, UINT64_MAX, m_PresentSemaphores[FrameIndex], nullptr, &m_CurrentImageIndex));
        return m_CurrentImageIndex;
    }


    void Swapchain::Present(const SharedPtr<CommandList>& CommandList)
    {
        VkSemaphore WaitSemaphore = CommandList->GetSemaphore();

        VkPresentInfoKHR PresentInfo{};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.pNext = nullptr;
        PresentInfo.pSwapchains = &m_Swapchain;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pWaitSemaphores = &WaitSemaphore;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pImageIndices = &m_CurrentImageIndex;

        VkCheck(vkQueuePresentKHR(m_RenderContext->GetGraphicsQueue().DeviceQueue, &PresentInfo));
    }

    VkImage Swapchain::GetImage(uint32_t Index)
    {
        if (Index > m_ImageCount)
        {
            SAPPHIRE_ENGINE_ERROR("Swapchain Image index out of bounds!");
            return nullptr;
        }

        return m_Images[Index];
    }

    VkImageView Swapchain::GetImageView(uint32_t Index)
    {
        if (Index > m_ImageCount)
        {
            SAPPHIRE_ENGINE_ERROR("Swapchain Image index out of bounds!");
            return nullptr;
        }

        return m_ImageViews[Index];
    }

    VkSemaphore Swapchain::GetSemaphore(uint32_t FrameIndex)
    {
        if (FrameIndex > m_PresentSemaphores.size())
        {
            SAPPHIRE_ENGINE_ERROR("Frame Index out of bounds!");
            return nullptr;
        }

        return m_PresentSemaphores[FrameIndex];
    }
}