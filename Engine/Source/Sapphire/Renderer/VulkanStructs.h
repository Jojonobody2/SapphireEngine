#pragma once

#include <vulkan/vulkan.h>
#include "Sapphire/Core/Application.h"

namespace Sapphire
{
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags Aspect, uint32_t BaseMipLevel = 0, uint32_t LevelCount = 1,
                                             uint32_t BaseArrayLayer = 0, uint32_t LayerCount = 1);

    //default: non mipmapped 2D color image
    VkImageViewCreateInfo ImageViewInfo(VkImage Image, VkFormat Format, VkImageViewType ViewType = VK_IMAGE_VIEW_TYPE_2D,
                                              VkImageSubresourceRange SubresourceRange = ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));

    VkCommandPoolCreateInfo CommandPoolInfo(uint32_t QueueFamily, VkCommandPoolCreateFlags Flags = 0);
    VkCommandBufferAllocateInfo CommandBufferAllocInfo(VkCommandPool CmdPool, VkCommandBufferLevel Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, uint32_t BufferCount = 1);
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags UsageFlags = 0);

    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer Cmd);
    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkSemaphore Semaphore, VkPipelineStageFlags2 PipelineStageFlags);
    VkSubmitInfo2 SubmitInfo(uint32_t WaitSemaphoreCount, VkSemaphoreSubmitInfo* pWaitSemaphoreInfos, uint32_t SignalSemaphoreCount, VkSemaphoreSubmitInfo* pSignalSemaphoreInfos,
                             uint32_t CommandBufferCount, VkCommandBufferSubmitInfo* pCommandBufferSubmitInfos);

    VkFenceCreateInfo FenceInfo(VkFenceCreateFlags Flags = 0);
    VkSemaphoreCreateInfo SemaphoreInfo();

    VkRenderingAttachmentInfo ColorAttachmentInfo(VkImageView ImageView, VkImageLayout ImageLayout, VkClearColorValue* ClearColor = nullptr);
    VkRenderingAttachmentInfo DepthAttachmentInfo(VkImageView ImageView, VkImageLayout ImageLayout, bool Clear = false);

    VkRenderingInfo RenderingInfo(uint32_t ColorAttachmentCount, VkRenderingAttachmentInfo* pColorAttachmentInfos, 
                                  VkRenderingAttachmentInfo* pDepthAttachment = nullptr,
                                  VkRect2D RenderingArea = { 0, 0,
                                                             Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() });

    VkViewport ViewportInfo(float Width = (float)Application::Get().GetWindow().GetWidth(), float Height = (float)Application::Get().GetWindow().
            GetHeight(), float X = 0, float Y = 0, float MinDepth = 0.f, float MaxDepth = 1.f);
    VkRect2D ScissorInfo(uint32_t Width = (uint32_t)Application::Get().GetWindow().GetWidth(), uint32_t Height = (uint32_t)Application::Get().GetWindow().
            GetHeight(), int32_t X = 0, int32_t Y = 0);
}