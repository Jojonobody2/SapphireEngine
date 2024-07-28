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

    void GPUMemoryAllocator::CopyBufferToImage(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUImage& Dst)
    {
        VkBufferImageCopy BufferImageCopy{};
        BufferImageCopy.bufferOffset = 0;
        BufferImageCopy.bufferRowLength = 0;
        BufferImageCopy.bufferImageHeight = 0;
        BufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        BufferImageCopy.imageSubresource.baseArrayLayer = 0;
        BufferImageCopy.imageSubresource.layerCount = 1;
        BufferImageCopy.imageSubresource.mipLevel = 0;
        BufferImageCopy.imageOffset = { 0, 0 };
        BufferImageCopy.imageExtent = { Dst.ImageSize.width, Dst.ImageSize.height, 1 };

        vkCmdCopyBufferToImage(Cmd, Src.Buffer, Dst.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &BufferImageCopy);
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

        VkCommandBuffer Cmd = UploadCmdList->Begin();
        CopyBufferToBuffer(Cmd, StagingBuffer, MeshBuffer.VertexBuffer.Buffer);
        CopyBufferToBuffer(Cmd, StagingBuffer, MeshBuffer.IndexBuffer, MeshData.Vertices.size() * sizeof(Vertex));
        UploadCmdList->Submit(nullptr);
        UploadCmdList->Wait();

        DestroyBuffer(StagingBuffer);

        MeshBuffer.DrawCount = MeshData.Indices.size();

        return MeshBuffer;
    }

    void GPUMemoryAllocator::DestroyMesh(const GPUMeshBuffer& MeshBuffer)
    {
        DestroyBuffer(MeshBuffer.IndexBuffer);
        DestroyBuffer(MeshBuffer.VertexBuffer);
    }

    GPUTexture GPUMemoryAllocator::UploadTexture(BitmapImage& Image, VkFormat Format)
    {
        GPUTexture Texture{};

        Texture.MipLevels = (uint32_t)std::floor(std::log2(std::max(Image.Width, Image.Height))) + 1;

        uint32_t ImageSize = Image.Pixels.size() * sizeof(uint8_t);

        GPUBuffer StagingBuffer = AllocateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, ImageSize);
        CopyDataToHost(StagingBuffer, Image.Pixels.data(), ImageSize);

        Texture.Image = AllocateImage({ Image.Width, Image.Height }, Format, VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0,
                Texture.MipLevels));

        SharedPtr<CommandList> UploadCmdList = CreateSharedPtr<CommandList>(m_RenderContext);

        VkCommandBuffer Cmd = UploadCmdList->Begin();
        {
            TransitionImageLayout(Cmd, Texture.Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, Texture.MipLevels));
            CopyBufferToImage(Cmd, StagingBuffer, Texture.Image);
            GenerateMipmaps(Cmd, Texture);
            TransitionImageLayout(Cmd, Texture.Image.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, Texture.MipLevels));
        }
        UploadCmdList->Submit(nullptr);
        UploadCmdList->Wait();

        DestroyBuffer(StagingBuffer);

        VkSamplerCreateInfo SamplerCreateInfo{};
        SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        SamplerCreateInfo.pNext = nullptr;
        SamplerCreateInfo.flags = 0;
        SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.anisotropyEnable = VK_FALSE;
        SamplerCreateInfo.maxAnisotropy = 0.f;
        SamplerCreateInfo.compareEnable = VK_FALSE;
        SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        SamplerCreateInfo.mipLodBias = 0.f;
        SamplerCreateInfo.minLod = 0.f;
        SamplerCreateInfo.maxLod = (float)Texture.MipLevels;
        SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        VkCheck(vkCreateSampler(m_RenderContext->GetDevice(), &SamplerCreateInfo, nullptr, &Texture.Sampler));

        return Texture;
    }

    void GPUMemoryAllocator::DestroyTexture(const GPUTexture& Texture)
    {
        vkDestroySampler(m_RenderContext->GetDevice(), Texture.Sampler, nullptr);
        DestroyImage(Texture.Image);
    }

    void GPUMemoryAllocator::GenerateMipmaps(VkCommandBuffer Cmd, const GPUTexture& Texture)
    {
        VkExtent2D CurrentSize = Texture.Image.ImageSize;

        for (int Mip = 0; Mip < Texture.MipLevels; Mip++)
        {
            VkExtent2D HalfSize = { CurrentSize.width / 2, CurrentSize.height / 2 };

            VkImageMemoryBarrier2 ImageMemoryBarrier{};
            ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            ImageMemoryBarrier.pNext = nullptr;
            ImageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            ImageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
            ImageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            ImageMemoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
            ImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            ImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ImageMemoryBarrier.image = Texture.Image.Image;
            ImageMemoryBarrier.subresourceRange = ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
            ImageMemoryBarrier.subresourceRange.levelCount = 1;
            ImageMemoryBarrier.subresourceRange.baseMipLevel = Mip;

            VkDependencyInfo ImageDependencyInfo{};
            ImageDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            ImageDependencyInfo.pNext = nullptr;
            ImageDependencyInfo.imageMemoryBarrierCount = 1;
            ImageDependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;

            vkCmdPipelineBarrier2(Cmd, &ImageDependencyInfo);

            if (Mip < Texture.MipLevels - 1)
            {
                VkImageBlit2 BlitRegion{};
                BlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
                BlitRegion.pNext = nullptr;
                BlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                BlitRegion.srcSubresource.baseArrayLayer = 0;
                BlitRegion.srcSubresource.layerCount = 1;
                BlitRegion.srcSubresource.mipLevel = Mip;
                BlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                BlitRegion.dstSubresource.baseArrayLayer = 0;
                BlitRegion.dstSubresource.layerCount = 1;
                BlitRegion.dstSubresource.mipLevel = Mip + 1;
                BlitRegion.srcOffsets[1].x = (int32_t)CurrentSize.width;
                BlitRegion.srcOffsets[1].y = (int32_t)CurrentSize.height;
                BlitRegion.srcOffsets[1].z = 1;
                BlitRegion.dstOffsets[1].x = (int32_t)HalfSize.width;
                BlitRegion.dstOffsets[1].y = (int32_t)HalfSize.height;
                BlitRegion.dstOffsets[1].z = 1;

                VkBlitImageInfo2 BlitImageInfo{};
                BlitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
                BlitImageInfo.pNext = nullptr;
                BlitImageInfo.srcImage = Texture.Image.Image;
                BlitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                BlitImageInfo.dstImage = Texture.Image.Image;
                BlitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                BlitImageInfo.regionCount = 1;
                BlitImageInfo.pRegions = &BlitRegion;
                BlitImageInfo.filter = VK_FILTER_LINEAR;

                vkCmdBlitImage2(Cmd, &BlitImageInfo);

                CurrentSize = HalfSize;
            }
        }
    }

    GPUTexture GPUMemoryAllocator::AllocateEmptyTexture(VkFormat Format, VkExtent2D Size, VkImageUsageFlags ImageUsage, VkImageAspectFlags ImageAspect)
    {
        GPUTexture Texture{};
        Texture.MipLevels = 1;

        Texture.Image = AllocateImage(Size, Format, ImageUsage | VK_IMAGE_USAGE_SAMPLED_BIT, ImageSubresourceRange(ImageAspect, 0,
                                                                                                                                                                 Texture.MipLevels));
        VkSamplerCreateInfo SamplerCreateInfo{};
        SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        SamplerCreateInfo.pNext = nullptr;
        SamplerCreateInfo.flags = 0;
        SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        SamplerCreateInfo.anisotropyEnable = VK_FALSE;
        SamplerCreateInfo.maxAnisotropy = 0.f;
        SamplerCreateInfo.compareEnable = VK_FALSE;
        SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        SamplerCreateInfo.mipLodBias = 0.f;
        SamplerCreateInfo.minLod = 0.f;
        SamplerCreateInfo.maxLod = 0.f;
        SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        VkCheck(vkCreateSampler(m_RenderContext->GetDevice(), &SamplerCreateInfo, nullptr, &Texture.Sampler));

        return Texture;
    }
}