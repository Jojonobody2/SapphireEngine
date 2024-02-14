#pragma once

#include "Descriptor.h"
#include "Types.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <vector>
#include <queue>
#include <functional>

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

	class Renderer
	{
	public:
		void Init();
		void Draw();
		void Shutdown();

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
		VkExtent2D m_DrawExtent{};
		DescriptorAllocator m_DescriptorAllocator{};

		VkDescriptorSet m_DrawImageDescriptor{};
		VkDescriptorSetLayout m_DrawImageDescriptorSetLayout{};

		VkPipeline m_GradientPipeline{};
		VkPipelineLayout m_GradientPipelineLayout{};

		DeletionQueue m_DeletionQueue{};

		ImmediateContext m_ImmediateContext{};

		FrameData m_FrameData[FRAMES_IN_FLIGTH];
		FrameData& GetCurrentFrame() { return m_FrameData[m_FrameIndex]; }

		uint32_t m_FrameIndex{};

		#ifdef SAPPHIRE_RENDER_DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger{};
		#endif
	};
}