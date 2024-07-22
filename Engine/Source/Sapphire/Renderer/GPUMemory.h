#pragma once

#include "RenderContext.h"
#include "GPUResources.h"
#include "Sapphire/Core/Base.h"

#include "../../ThirdParty/VulkanMemoryAllocator/include/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Sapphire
{
    class GPUMemoryAllocator
    {
    public:
        explicit GPUMemoryAllocator(const SharedPtr<RenderContext>& RenderContext);
        virtual ~GPUMemoryAllocator();

        GPUImage AllocateImage(VkExtent2D ImageSize, VkFormat ImageFormat, VkImageUsageFlags ImageUsage,
                               VkImageSubresourceRange SubresourceRange, VkImageType ImageType = VK_IMAGE_TYPE_2D,
                               VkImageViewType ImageViewType = VK_IMAGE_VIEW_TYPE_2D);
        void DestroyImage(const GPUImage& Image);

        GPUBuffer AllocateBuffer(VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage, size_t BufferSize);
        GPUBufferAddressable AllocateBufferAddressable(VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage, size_t BufferSize);
        void DestroyBuffer(const GPUBuffer& Buffer);
        void DestroyBuffer(const GPUBufferAddressable& Buffer);

        void CopyDataToHost(const GPUBuffer& Buffer, void* pData, size_t DataSize);
        void CopyBufferToBuffer(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUBuffer& Dst);

    private:
        VmaAllocator m_Allocator{};

    private:
        SharedPtr<RenderContext> m_RenderContext{};
    };
}