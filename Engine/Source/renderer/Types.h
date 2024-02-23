#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <glm/glm.hpp>

#include <functional>
#include <string>

namespace Sapphire
{
	struct AllocatedImage
	{
		VkImage Image;
		VkImageView ImageView;
		VmaAllocation Allocation;
		VkExtent3D ImageExtent;
		VkFormat ImageFormat;
	};

	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo AllocationInfo;
	};

	struct GpuMeshBuffer
	{
		AllocatedBuffer VertexBuffer;
		AllocatedBuffer IndexBuffer;
		VkDeviceAddress VertexBufferAddress;
	};

	struct GpuDrawPushConstants
	{
		glm::mat4 WorldMatrix;
		VkDeviceAddress VertexBuffer;
	};

	struct Vertex
	{
		alignas(16) glm::vec3 Position;
	};

	struct GeoSurface
	{
		uint32_t StartIndex;
		uint32_t Count;
	};

	struct MeshAsset
	{
		std::string Name;
		std::vector<GeoSurface> Surfaces;
		GpuMeshBuffer Meshbuffer;
	};

	class ImmediateContext
	{
	public:
		void CreateImmediateContext(VkDevice Device, uint32_t QueueIndex, VkQueue Queue);
		void SubmitImmediate(VkDevice Device, std::function<void(VkCommandBuffer Cmd)>&& Function);
		void DestroyImmediateContext(VkDevice Device);

	private:
		VkFence m_ImmediateFence;
		VkCommandBuffer m_ImmediateCommandBuffer;
		VkCommandPool m_ImmediateCommandPool;
		VkQueue m_ImmediateQueue;
	};
}