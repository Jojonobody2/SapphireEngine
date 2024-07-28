#pragma once

#include "RenderContext.h"
#include "GPUResources.h"
#include "Sapphire/Core/Base.h"
#include "Sapphire/Import/Model/ModelImporter.h"
#include "Sapphire/Import/Image/ImageImporter.h"

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

        void CopyDataToHost(const GPUBuffer& Buffer, void* pData, size_t DataSize, size_t Offset = 0);
        static void CopyBufferToBuffer(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUBuffer& Dst, size_t SrcOffset = 0,
                                size_t DstOffset = 0);
        static void CopyBufferToImage(VkCommandBuffer Cmd, const GPUBuffer& Src, const GPUImage& Dst);

        GPUMeshBuffer UploadMesh(MeshData& ModelData);
        void DestroyMesh(const GPUMeshBuffer& MeshBuffer);

        GPUTexture AllocateEmptyTexture(VkFormat Format, VkExtent2D Size, VkImageUsageFlags ImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VkImageAspectFlags ImageAspect =
                VK_IMAGE_ASPECT_COLOR_BIT);
        GPUTexture UploadTexture(BitmapImage& Image, VkFormat Format);
        void DestroyTexture(const GPUTexture& Texture);

        static void GenerateMipmaps(VkCommandBuffer Cmd, const GPUTexture& Texture);

    private:
        VmaAllocator m_Allocator{};

    private:
        SharedPtr<RenderContext> m_RenderContext{};
    };
}