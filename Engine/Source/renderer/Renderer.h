#pragma once

#include "Descriptor.h"
#include "Types.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <glm/glm.hpp>

#include <vector>
#include <queue>
#include <functional>
#include <memory>

constexpr auto FRAMES_IN_FLIGTH = 2;

namespace Sapphire
{
	struct DeletionQueue
	{
		std::deque<std::function<void()>> Deletors;

		void PushFunction(std::function<void()>&& Function)
		{
			Deletors.push_back(Function);
		}

		void Flush()
		{
			for (auto Iterator = Deletors.rbegin(); Iterator != Deletors.rend(); Iterator++)
			{
				(*Iterator)();
			}

			Deletors.clear();
		}
	};

	struct FrameData
	{
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
		VkSemaphore PresentSemaphore, RenderSemaphore;
		VkFence RenderFence;

		DeletionQueue DeletionQueue;
	};

	struct GradientPushConstants
	{
		glm::vec4 data1;
		glm::vec4 data2;
		glm::vec4 data3;
		glm::vec4 data4;
	};

	class Renderer
	{
	public:
		void Init();
		void Draw();
		void Shutdown();

		GpuMeshBuffer UploadMesh(std::span<Vertex> Vertices, std::span<uint32_t> Indices);

	private:
		VkInstance m_Instance{};
		VkSurfaceKHR m_Surface{};
		VkPhysicalDevice m_PhysicalDevice{};
		VkDevice m_Device{};
		VkQueue m_GraphicsQueue{};
		VkQueue m_PresentQueue{};
		VkSwapchainKHR m_Swapchain{};
		VkExtent2D m_SwapchainExtent{};
		std::vector<VkImage> m_SwapchainImages{};
		std::vector<VkImageView> m_SwapchainImageViews{};
		VmaAllocator m_Allocator{};
		AllocatedImage m_DrawImage{};
		AllocatedImage m_DepthImage{};
		VkExtent2D m_DrawExtent{};
		DescriptorAllocator m_DescriptorAllocator{};

		VkDescriptorSet m_DrawImageDescriptor{};
		VkDescriptorSetLayout m_DrawImageDescriptorSetLayout{};

		VkPipeline m_GradientPipeline{};
		VkPipelineLayout m_GradientPipelineLayout{};

		VkPipeline m_TrianglePipeline{};
		VkPipelineLayout m_TrianglePipelineLayout{};

		std::vector<std::shared_ptr<MeshAsset>> m_TestMeshes;

		DeletionQueue m_DeletionQueue{};

		ImmediateContext m_ImmediateContext{};

		FrameData m_FrameData[FRAMES_IN_FLIGTH];
		FrameData& GetCurrentFrame() { return m_FrameData[m_FrameIndex]; }

		bool ResizeRequested = false;

		uint32_t m_FrameIndex{};

		AllocatedBuffer CreateBuffer(size_t BufferSize, VkBufferUsageFlags BufferUsage, VmaMemoryUsage BufferMemoryUsage);
		void DestroyBuffer(const AllocatedBuffer& Buffer);

		void CreateSwapchain();
		void ResizeSwapchain();

		#ifdef SAPPHIRE_RENDER_DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger{};
		#endif
	};
}