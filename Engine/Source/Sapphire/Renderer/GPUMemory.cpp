#include "GPUMemory.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

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

    void GPUMemoryAllocator::CopyDataToHost(const GPUBuffer& Buffer, void* pData, size_t DataSize, size_t Offset)
    {
        void* pMappedData;
        vmaMapMemory(m_Allocator, Buffer.BufferAllocation, &pMappedData);
        memcpy((char*)pMappedData + Offset, pData, DataSize);
        vmaUnmapMemory(m_Allocator, Buffer.BufferAllocation);
    }

    void GPUMemoryAllocator::CopyBufferToBuffer(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUBuffer& Dst, size_t SrcOffset,
                                                size_t DstOffset)
    {
        VkBufferCopy BufferCopy{};
        BufferCopy.srcOffset = SrcOffset;
        BufferCopy.dstOffset = DstOffset;
        BufferCopy.size = Dst.BufferSize;

        vkCmdCopyBuffer(Cmd, Src.Buffer, Dst.Buffer, 1, &BufferCopy);
    }

    GPUMeshBuffer GPUMemoryAllocator::UploadMesh(MeshData& MeshData)
    {
        GPUMeshBuffer MeshBuffer{};

        GPUBuffer StagingBuffer = AllocateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
            MeshData.Vertices.size() * sizeof(Vertex) + MeshData.Indices.size() * sizeof(uint32_t));
        CopyDataToHost(StagingBuffer, MeshData.Vertices.data(), MeshData.Vertices.size() * sizeof(Vertex));
        CopyDataToHost(StagingBuffer, MeshData.Indices.data(), MeshData.Indices.size() *
            sizeof(uint32_t), MeshData.Vertices.size() * sizeof(Vertex));

        MeshBuffer.VertexBuffer = AllocateBufferAddressable(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, MeshData.Vertices.size() * sizeof(Vertex));

        MeshBuffer.IndexBuffer = AllocateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, MeshData.Indices.size() * sizeof(uint32_t));

        SharedPtr<CommandList> UploadCmdList = CreateSharedPtr<CommandList>(m_RenderContext);

        UploadCmdList->Wait();
        VkCommandBuffer Cmd = UploadCmdList->Begin();
        CopyBufferToBuffer(Cmd, StagingBuffer, MeshBuffer.VertexBuffer.Buffer);
        CopyBufferToBuffer(Cmd, StagingBuffer, MeshBuffer.IndexBuffer, MeshData.Vertices.size() * sizeof(Vertex));
        UploadCmdList->Submit(nullptr);
        UploadCmdList->Wait();

        DestroyBuffer(StagingBuffer);

        return MeshBuffer;
    }

    void GPUMemoryAllocator::DestroyMesh(const GPUMeshBuffer& MeshBuffer)
    {
        DestroyBuffer(MeshBuffer.IndexBuffer);
        DestroyBuffer(MeshBuffer.VertexBuffer);
    }
}