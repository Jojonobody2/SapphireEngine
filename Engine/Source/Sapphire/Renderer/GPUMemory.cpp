#include "GPUMemory.h"

#define VMA_IMPLEMENTATION
#include "../../ThirdParty/VulkanMemoryAllocator/include/vk_mem_alloc.h"

#include "VulkanUtil.h"
#include "VulkanStructs.h"

namespace Sapphire
{
    GPUMemoryAllocator::GPUMemoryAllocator(const SharedPtr<RenderContext>& RenderContext)
    {
        m_RenderContext = RenderContext;

        VmaAllocatorCreateInfo AllocatorCreateInfo{};
        AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        AllocatorCreateInfo.instance = m_RenderContext->GetInstance();
        AllocatorCreateInfo.physicalDevice = m_RenderContext->GetPhysicalDevice();
        AllocatorCreateInfo.device = m_RenderContext->GetDevice();

        VkCheck(vmaCreateAllocator(&AllocatorCreateInfo, &m_Allocator));
    }

    GPUMemoryAllocator::~GPUMemoryAllocator()
    {
        vmaDestroyAllocator(m_Allocator);
    }

    GPUImage GPUMemoryAllocator::AllocateImage(VkExtent2D ImageSize, VkFormat ImageFormat, VkImageUsageFlags ImageUsage, VkImageSubresourceRange SubresourceRange, VkImageType ImageType,
                                               VkImageViewType ImageViewType)
    {
        VkImageCreateInfo ImageCreateInfo{};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.pNext = nullptr;
        ImageCreateInfo.flags = 0;
        ImageCreateInfo.imageType = ImageType;
        ImageCreateInfo.format = ImageFormat;
        ImageCreateInfo.extent = { ImageSize.width, ImageSize.height, 1 };
        ImageCreateInfo.mipLevels = SubresourceRange.levelCount;
        ImageCreateInfo.arrayLayers = SubresourceRange.layerCount;
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        ImageCreateInfo.usage = ImageUsage;

        VmaAllocationCreateInfo AllocationCreateInfo{};
        AllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        AllocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        GPUImage Image{};
        VkCheck(vmaCreateImage(m_Allocator, &ImageCreateInfo, &AllocationCreateInfo, &Image.Image, &Image.ImageAllocation, &Image.ImageAllocationInfo));

        VkImageViewCreateInfo ImageViewCreateInfo = ImageViewInfo(Image.Image, ImageFormat, ImageViewType, SubresourceRange);
        VkCheck(vkCreateImageView(m_RenderContext->GetDevice(), &ImageViewCreateInfo, nullptr, &Image.ImageView));

        Image.ImageFormat = ImageFormat;
        Image.ImageSize = ImageSize;

        return Image;
    }

    void GPUMemoryAllocator::DestroyImage(const GPUImage& Image)
    {
        vkDestroyImageView(m_RenderContext->GetDevice(), Image.ImageView, nullptr);
        vmaDestroyImage(m_Allocator, Image.Image, Image.ImageAllocation);
    }

    GPUBuffer GPUMemoryAllocator::AllocateBuffer(VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage, size_t BufferSize)
    {
        VkBufferCreateInfo BufferCreateInfo{};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.pNext = nullptr;
        BufferCreateInfo.flags = 0;
        BufferCreateInfo.size = (VkDeviceSize)BufferSize;
        BufferCreateInfo.usage = BufferUsage;

        VmaAllocationCreateInfo AllocationCreateInfo{};
        AllocationCreateInfo.usage = MemoryUsage;

        GPUBuffer Buffer{};
        VkCheck(vmaCreateBuffer(m_Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer.Buffer, &Buffer.BufferAllocation, &Buffer.BufferAllocationInfo));

        Buffer.BufferSize = BufferSize;

        return Buffer;
    }

    GPUBufferAddressable GPUMemoryAllocator::AllocateBufferAddressable(VkBufferUsageFlags BufferUsage, VmaMemoryUsage MemoryUsage,
                                                                       size_t BufferSize)
    {
        VkBufferCreateInfo BufferCreateInfo{};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.pNext = nullptr;
        BufferCreateInfo.flags = 0;
        BufferCreateInfo.size = (VkDeviceSize)BufferSize;
        BufferCreateInfo.usage = BufferUsage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

        VmaAllocationCreateInfo AllocationCreateInfo{};
        AllocationCreateInfo.usage = MemoryUsage;

        GPUBufferAddressable Buffer{};
        VkCheck(vmaCreateBuffer(m_Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer.Buffer.Buffer,
                                &Buffer.Buffer.BufferAllocation, &Buffer.Buffer.BufferAllocationInfo));

        VkBufferDeviceAddressInfo BufferDeviceAddressInfo{};
        BufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        BufferDeviceAddressInfo.pNext = nullptr;
        BufferDeviceAddressInfo.buffer = Buffer.Buffer.Buffer;

        Buffer.BufferAddress = vkGetBufferDeviceAddress(m_RenderContext->GetDevice(), &BufferDeviceAddressInfo);
        Buffer.Buffer.BufferSize = BufferSize;

        return Buffer;
    }

    void GPUMemoryAllocator::DestroyBuffer(const GPUBuffer& Buffer)
    {
        vmaDestroyBuffer(m_Allocator, Buffer.Buffer, Buffer.BufferAllocation);
    }

    void GPUMemoryAllocator::DestroyBuffer(const GPUBufferAddressable& Buffer)
    {
        vmaDestroyBuffer(m_Allocator, Buffer.Buffer.Buffer, Buffer.Buffer.BufferAllocation);
    }

    void GPUMemoryAllocator::CopyDataToHost(const GPUBuffer& Buffer, void* pData, size_t DataSize)
    {
        void* pMappedData;
        vmaMapMemory(m_Allocator, Buffer.BufferAllocation, &pMappedData);
        memcpy(pMappedData, pData, DataSize);
        vmaUnmapMemory(m_Allocator, Buffer.BufferAllocation);
    }

    void GPUMemoryAllocator::CopyBufferToBuffer(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUBuffer& Dst)
    {
        VkBufferCopy BufferCopy{};
        BufferCopy.srcOffset = 0;
        BufferCopy.dstOffset = 0;
        BufferCopy.size = Src.BufferSize;

        vkCmdCopyBuffer(Cmd, Src.Buffer, Dst.Buffer, 1, &BufferCopy);
    }
}