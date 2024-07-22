#include "VulkanStructs.h"

namespace Sapphire
{
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags Aspect, uint32_t BaseMipLevel, uint32_t LevelCount, uint32_t BaseArrayLayer,
                                             uint32_t LayerCount)
    {
        VkImageSubresourceRange SubresourceRange{};
        SubresourceRange.aspectMask = Aspect;
        SubresourceRange.baseMipLevel = BaseMipLevel;
        SubresourceRange.levelCount = LevelCount;
        SubresourceRange.baseArrayLayer = BaseArrayLayer;
        SubresourceRange.layerCount = LayerCount;

        return SubresourceRange;
    }

    VkImageViewCreateInfo ImageViewInfo(VkImage Image, VkFormat Format, VkImageViewType ViewType, VkImageSubresourceRange SubresourceRange)
    {
        VkImageViewCreateInfo ImageViewCreateInfo{};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.pNext = nullptr;
        ImageViewCreateInfo.flags = 0;
        ImageViewCreateInfo.image = Image;
        ImageViewCreateInfo.viewType = ViewType;
        ImageViewCreateInfo.format = Format;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        ImageViewCreateInfo.subresourceRange = SubresourceRange;

        return ImageViewCreateInfo;
    }

    VkSemaphoreCreateInfo SemaphoreInfo()
    {
        VkSemaphoreCreateInfo SemaphoreCreateInfo{};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        SemaphoreCreateInfo.pNext = nullptr;
        SemaphoreCreateInfo.flags = 0;

        return SemaphoreCreateInfo;
    }

    VkCommandPoolCreateInfo CommandPoolInfo(uint32_t QueueFamily, VkCommandPoolCreateFlags Flags)
    {
        VkCommandPoolCreateInfo CommandPoolCreateInfo{};
        CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CommandPoolCreateInfo.pNext = nullptr;
        CommandPoolCreateInfo.flags = Flags;
        CommandPoolCreateInfo.queueFamilyIndex = QueueFamily;

        return CommandPoolCreateInfo;
    }

    VkCommandBufferAllocateInfo CommandBufferAllocInfo(VkCommandPool CmdPool, VkCommandBufferLevel Level, uint32_t BufferCount)
    {
        VkCommandBufferAllocateInfo CommandBufferAllocInfo{};
        CommandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocInfo.pNext = nullptr;
        CommandBufferAllocInfo.commandPool = CmdPool;
        CommandBufferAllocInfo.level = Level;
        CommandBufferAllocInfo.commandBufferCount = BufferCount;

        return CommandBufferAllocInfo;
    }

    VkFenceCreateInfo FenceInfo(VkFenceCreateFlags Flags)
    {
        VkFenceCreateInfo FenceCreateInfo{};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.pNext = nullptr;
        FenceCreateInfo.flags = Flags;

        return FenceCreateInfo;
    }

    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags UsageFlags)
    {
        VkCommandBufferBeginInfo CommandBufferBeginInfo{};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.pNext = nullptr;
        CommandBufferBeginInfo.pInheritanceInfo = nullptr;
        CommandBufferBeginInfo.flags = UsageFlags;

        return CommandBufferBeginInfo;
    }

    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer Cmd)
    {
        VkCommandBufferSubmitInfo CommandBufferSubmitInfo{};
        CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        CommandBufferSubmitInfo.pNext = nullptr;
        CommandBufferSubmitInfo.commandBuffer = Cmd;
        CommandBufferSubmitInfo.deviceMask = 0;

        return CommandBufferSubmitInfo;
    }

    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkSemaphore Semaphore, VkPipelineStageFlags2 PipelineStageFlags)
    {
        VkSemaphoreSubmitInfo SemaphoreSubmitInfo{};
        SemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        SemaphoreSubmitInfo.pNext = nullptr;
        SemaphoreSubmitInfo.semaphore = Semaphore;
        SemaphoreSubmitInfo.stageMask = PipelineStageFlags;
        SemaphoreSubmitInfo.deviceIndex = 0;
        SemaphoreSubmitInfo.value = 1;

        return SemaphoreSubmitInfo;
    }

    VkSubmitInfo2 SubmitInfo(uint32_t WaitSemaphoreCount, VkSemaphoreSubmitInfo *pWaitSemaphoreInfos, uint32_t SignalSemaphoreCount,
               VkSemaphoreSubmitInfo *pSignalSemaphoreInfos, uint32_t CommandBufferCount,
               VkCommandBufferSubmitInfo *pCommandBufferSubmitInfos)
    {
        VkSubmitInfo2 SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        SubmitInfo.pNext = nullptr;
        SubmitInfo.waitSemaphoreInfoCount = WaitSemaphoreCount;
        SubmitInfo.pWaitSemaphoreInfos = pWaitSemaphoreInfos;
        SubmitInfo.signalSemaphoreInfoCount = SignalSemaphoreCount;
        SubmitInfo.pSignalSemaphoreInfos = pSignalSemaphoreInfos;
        SubmitInfo.commandBufferInfoCount = CommandBufferCount;
        SubmitInfo.pCommandBufferInfos = pCommandBufferSubmitInfos;

        return SubmitInfo;
    }

    VkRenderingAttachmentInfo ColorAttachmentInfo(VkImageView ImageView, VkImageLayout ImageLayout, VkClearColorValue* ClearColor)
    {
        VkRenderingAttachmentInfo AttachmentInfo{};
        AttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        AttachmentInfo.pNext = nullptr;
        if (ClearColor) AttachmentInfo.clearValue.color = *ClearColor;
        AttachmentInfo.imageLayout = ImageLayout;
        AttachmentInfo.imageView = ImageView;
        AttachmentInfo.loadOp = ClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        AttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        return AttachmentInfo;
    }

    VkRenderingInfo RenderingInfo(uint32_t ColorAttachmentCount, VkRenderingAttachmentInfo *pColorAttachmentInfos, VkRect2D RenderingArea)
    {
        VkRenderingInfo RenderingInfo{};
        RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        RenderingInfo.pNext = nullptr;
        RenderingInfo.flags = 0;
        RenderingInfo.colorAttachmentCount = ColorAttachmentCount;
        RenderingInfo.pColorAttachments = pColorAttachmentInfos;
        RenderingInfo.renderArea = RenderingArea;
        RenderingInfo.layerCount = 1;

        return RenderingInfo;
    }
}