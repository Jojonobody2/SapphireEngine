#pragma once

#include <vulkan/vulkan.h>

#include "../../ThirdParty/VulkanMemoryAllocator/include/vk_mem_alloc.h"

namespace Sapphire
{
    struct GPUImage
    {
        VkImage Image;
        VkImageView ImageView;
        VkFormat ImageFormat;
        VmaAllocation ImageAllocation;
        VmaAllocationInfo ImageAllocationInfo;
        VkExtent2D ImageSize;
    };

    struct GPUBuffer
    {
        VkBuffer Buffer;
        VmaAllocation BufferAllocation;
        VmaAllocationInfo BufferAllocationInfo{};
        size_t BufferSize;
    };

    struct GPUBufferAddressable
    {
        GPUBuffer Buffer;
        VkDeviceAddress BufferAddress;
    };

    struct GPUMeshBuffer
    {
        GPUBufferAddressable VertexBuffer;
        GPUBuffer IndexBuffer;
    };
}