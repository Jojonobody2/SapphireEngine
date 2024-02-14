#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <functional>

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