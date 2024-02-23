#pragma once

#include "Types.h"

#include "core/Logger.h"
#include "core/Application.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <optional>

#define VkCheckResult(Result) if (Result != 0) SAPPHIRE_ERROR("Render Error: {} {} {}", __FILE__, __LINE__, Result)

namespace Sapphire
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsCompatible() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapchainInfo
	{
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> SurfaceFormats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	bool IsLayerSupported(const char* Layer);
	std::vector<const char*> GetRequiredInstanceExtensions();

	VkSurfaceKHR CreateSurface(VkInstance Instance);

	VkPhysicalDevice SelectPhysicalDevice(VkInstance Instance, VkSurfaceKHR Surface);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	int RateDevice(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);

	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> SurfaceFormats);
	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> PresentModes);
	VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& SurfaceCapabilities);

	SwapchainInfo GetSwapchainInfo(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);

	VkImageSubresourceRange CreateImageSubresourceRange(VkImageAspectFlags AspectMask);
	VkCommandBufferSubmitInfo CreateCmdSubmitInfo(VkCommandBuffer Cmd);
	VkSemaphoreSubmitInfo CreateSemaphoreSubmitInfo(VkPipelineStageFlags2 PipelineStageMask, VkSemaphore Semaphore);
	VkSubmitInfo2 CreateSubmitInfo(VkCommandBufferSubmitInfo* CmdSubmitInfo, VkSemaphoreSubmitInfo* SignalSemaphoreInfo,
		VkSemaphoreSubmitInfo* WaitSemaphoreInfo);
	VkImageCreateInfo CreateImageCreateInfo(VkFormat ImageFormat, VkImageUsageFlags ImageUsage, VkExtent3D ImageExtent);
	VkImageViewCreateInfo CreateImageViewCreateInfo(VkFormat ImageFormat, VkImage Image, VkImageAspectFlags ImageAspect);

	VkRenderingAttachmentInfo ColorAttachmentInfo(VkImageView ImageView, VkClearValue* ClearValue, VkImageLayout ImageLayout);
	VkRenderingAttachmentInfo DepthAttachmentInfo(VkImageView ImageView, VkImageLayout ImageLayout);

	void TransitionImage(VkCommandBuffer Cmd, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NewLayout);
	void CopyImageToImage(VkCommandBuffer Cmd, VkImage SrcImage, VkImage DstImage, VkExtent2D SrcSize, VkExtent2D DstSize);

	bool LoadShaderModule(const char* Path, VkDevice Device, VkShaderModule* ShaderModule);

	#ifdef SAPPHIRE_RENDER_DEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	VkResult vkCreateDebugUtilsMessenger(VkInstance Instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* DebugUtilsMessengerCreateInfo, const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	
	void vkDestroyDebugUtilsMessenger(VkInstance Instance, const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT DebugMessenger);
	#endif
}
