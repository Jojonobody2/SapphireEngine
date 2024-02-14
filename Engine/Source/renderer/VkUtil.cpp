#include "VkUtil.h"

#if defined(SAPPHIRE_WINDOWS) || defined(SAPPHIRE_LINUX)
	#include <GLFW/glfw3.h>
#else
	#error "OS not supported!"
#endif

#include <fstream>

namespace Sapphire
{
	bool IsLayerSupported(const char* Layer)
	{
		uint32_t LayerCount{};
		vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

		std::vector<VkLayerProperties> AvailableLayers(LayerCount);
		vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

		for (const VkLayerProperties& LayerProperties : AvailableLayers)
		{

			if (strcmp(Layer, LayerProperties.layerName) == 0)
			{
				return true;
			}
		}

		return false;
	}

	std::vector<const char*> GetRequiredInstanceExtensions()
	{
		std::vector<const char*> RequiredInstanceExtensions{};

	#if defined(SAPPHIRE_WINDOWS) || defined(SAPPHIRE_LINUX)
		uint32_t GLFWInstanceExtensionCount{};
		const char** GLFWInstanceExtensions = glfwGetRequiredInstanceExtensions(&GLFWInstanceExtensionCount);
		RequiredInstanceExtensions = std::vector<const char*>(
			GLFWInstanceExtensions, GLFWInstanceExtensions + GLFWInstanceExtensionCount);
	#else
		#error "OS not supported!"
	#endif

		return RequiredInstanceExtensions;
	}

	VkSurfaceKHR CreateSurface(VkInstance Instance)
	{
		VkSurfaceKHR Surface;

	#if defined(SAPPHIRE_WINDOWS) || defined(SAPPHIRE_LINUX)
		VkCheckResult(glfwCreateWindowSurface(Instance, (GLFWwindow*)Application::Get().GetWindow()->GetNativeWindow(),
			nullptr, &Surface));
	#else
		#error "OS not supported!"
	#endif

		return Surface;
	}

	VkPhysicalDevice SelectPhysicalDevice(VkInstance Instance, VkSurfaceKHR Surface)
	{
		uint32_t PhysicalDeviceCount{};
		vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, nullptr);

		std::vector<VkPhysicalDevice> m_PhysicalDevices(PhysicalDeviceCount);
		vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, m_PhysicalDevices.data());

		std::vector<std::pair<VkPhysicalDevice, int>> PhysicalDeviceRatings{};

		for (const VkPhysicalDevice& PhysicalDevice : m_PhysicalDevices)
		{
			PhysicalDeviceRatings.push_back(std::make_pair(PhysicalDevice, RateDevice(PhysicalDevice, Surface)));
		}

		std::sort(PhysicalDeviceRatings.begin(), PhysicalDeviceRatings.end(), [=](std::pair<VkPhysicalDevice, int>& A,
			std::pair<VkPhysicalDevice, int>& B)
			{
				return A.second > B.second;
			});

		VkPhysicalDevice PhysicalDevice{};

		if (PhysicalDeviceRatings[0].second > 0)
		{
			PhysicalDevice = PhysicalDeviceRatings[0].first;
		}
		else
		{
			SAPPHIRE_ERROR("No suitable GPU found!");
		}

		return PhysicalDevice;
	}

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
	{
		QueueFamilyIndices QueueFamilyIndices{};

		uint32_t QueueFamilyCount{};
		vkGetPhysicalDeviceQueueFamilyProperties2(PhysicalDevice, &QueueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties2> QueueFamilyProperties(QueueFamilyCount);
		for (int i = 0; i < QueueFamilyProperties.size(); i++)
			QueueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;

		vkGetPhysicalDeviceQueueFamilyProperties2(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

		int i{};
		for (const VkQueueFamilyProperties2& QueueFamilyProperty : QueueFamilyProperties)
		{
			if (QueueFamilyProperty.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				QueueFamilyIndices.GraphicsFamily = i;
			}

			VkBool32 PresentSupported{};
			vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &PresentSupported);

			if (PresentSupported)
			{
				QueueFamilyIndices.PresentFamily = i;
			}

			i++;
		}

		return QueueFamilyIndices;
	}

	int RateDevice(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
	{
		int Rating{};

		VkPhysicalDeviceProperties2 DeviceProperties{};
		DeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vkGetPhysicalDeviceProperties2(PhysicalDevice, &DeviceProperties);

		VkPhysicalDeviceFeatures2 DeviceFeatures{};
		DeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vkGetPhysicalDeviceFeatures2(PhysicalDevice, &DeviceFeatures);

		QueueFamilyIndices QueueFamilyIndices = FindQueueFamilies(PhysicalDevice, Surface);
		if (!QueueFamilyIndices.IsCompatible())
			return 0;

		if (DeviceProperties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			Rating += 2000;

		Rating += DeviceProperties.properties.limits.maxImageDimension2D / 10;

		VkPhysicalDeviceMemoryProperties2 MemoryProperties{};
		MemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

		vkGetPhysicalDeviceMemoryProperties2(PhysicalDevice, &MemoryProperties);

		VkMemoryHeap* MemoryHeapsPointer = MemoryProperties.memoryProperties.memoryHeaps;
		std::vector<VkMemoryHeap> MemoryHeaps = std::vector<VkMemoryHeap>(MemoryHeapsPointer,
			MemoryHeapsPointer + MemoryProperties.memoryProperties.memoryHeapCount);

		for (const VkMemoryHeap& MemoryHeap : MemoryHeaps)
		{
			if (MemoryHeap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				Rating += (uint32_t)MemoryHeap.size / 10000000;
			}
		}

		return Rating;
	}

	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> SurfaceFormats)
	{
		for (const VkSurfaceFormatKHR SurfaceFormat : SurfaceFormats)
		{
			if (SurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
				SurfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return SurfaceFormat;
			}
		}

		return SurfaceFormats[0];
	}

	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> PresentModes)
	{
		for (const VkPresentModeKHR PresentMode : PresentModes)
		{
			if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return PresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& SurfaceCapabilities)
	{
		if (SurfaceCapabilities.currentExtent.width != UINT32_MAX)
		{
			return SurfaceCapabilities.currentExtent;
		}
		else
		{
			uint32_t Width = Application::Get().GetWindow()->GetWidth();
			uint32_t Height = Application::Get().GetWindow()->GetHeight();

			VkExtent2D Extent = { Width, Height };

			Extent.width = std::clamp(Extent.width, SurfaceCapabilities.minImageExtent.width,
				SurfaceCapabilities.maxImageExtent.width);
			Extent.height = std::clamp(Extent.height, SurfaceCapabilities.minImageExtent.height,
				SurfaceCapabilities.maxImageExtent.height);

			return Extent;
		}
	}

	SwapchainInfo GetSwapchainInfo(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface)
	{
		SwapchainInfo Info{};
		
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &Info.SurfaceCapabilities);

		uint32_t SurfaceFormatCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, nullptr);
		Info.SurfaceFormats.resize(SurfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, Info.SurfaceFormats.data());

		uint32_t PresentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
		Info.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, Info.PresentModes.data());

		return Info;
	}

	VkImageSubresourceRange CreateImageSubresourceRange(VkImageAspectFlags AspectMask)
	{
		VkImageSubresourceRange ImageSubresourceRange{};
		ImageSubresourceRange.aspectMask = AspectMask;
		ImageSubresourceRange.baseMipLevel = 0;
		ImageSubresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		ImageSubresourceRange.baseArrayLayer = 0;
		ImageSubresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		return ImageSubresourceRange;
	}

	VkCommandBufferSubmitInfo CreateCmdSubmitInfo(VkCommandBuffer Cmd)
	{
		VkCommandBufferSubmitInfo CmdSubitInfo{};
		CmdSubitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		CmdSubitInfo.pNext = nullptr;
		CmdSubitInfo.commandBuffer = Cmd;
		CmdSubitInfo.deviceMask = 0;

		return CmdSubitInfo;
	}

	VkSemaphoreSubmitInfo CreateSemaphoreSubmitInfo(VkPipelineStageFlags2 PipelineStageMask, VkSemaphore Semaphore)
	{
		VkSemaphoreSubmitInfo SemaphoreSubmitInfo{};
		SemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		SemaphoreSubmitInfo.pNext = nullptr;
		SemaphoreSubmitInfo.semaphore = Semaphore;
		SemaphoreSubmitInfo.stageMask = PipelineStageMask;
		SemaphoreSubmitInfo.deviceIndex = 0;
		SemaphoreSubmitInfo.value = 1;

		return SemaphoreSubmitInfo;
	}

	VkSubmitInfo2 CreateSubmitInfo(VkCommandBufferSubmitInfo* CmdSubmitInfo, VkSemaphoreSubmitInfo* SignalSemaphoreInfo, 
		VkSemaphoreSubmitInfo* WaitSemaphoreInfo)
	{
		VkSubmitInfo2 SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		SubmitInfo.pNext = nullptr;
		SubmitInfo.waitSemaphoreInfoCount = WaitSemaphoreInfo == nullptr ? 0 : 1;
		SubmitInfo.pWaitSemaphoreInfos = WaitSemaphoreInfo;
		SubmitInfo.signalSemaphoreInfoCount = SignalSemaphoreInfo == nullptr ? 0 : 1;
		SubmitInfo.pSignalSemaphoreInfos = SignalSemaphoreInfo;
		SubmitInfo.commandBufferInfoCount = 1;
		SubmitInfo.pCommandBufferInfos = CmdSubmitInfo;

		return SubmitInfo;
	}

	VkImageCreateInfo CreateImageCreateInfo(VkFormat ImageFormat, VkImageUsageFlags ImageUsage, VkExtent3D ImageExtent)
	{
		VkImageCreateInfo ImageCreateInfo{};
		ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageCreateInfo.pNext = nullptr;
		ImageCreateInfo.flags = 0;
		ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageCreateInfo.format = ImageFormat;
		ImageCreateInfo.extent = ImageExtent;
		ImageCreateInfo.mipLevels = 1;
		ImageCreateInfo.arrayLayers = 1;
		ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		ImageCreateInfo.usage = ImageUsage;

		return ImageCreateInfo;
	}

	VkImageViewCreateInfo CreateImageViewCreateInfo(VkFormat ImageFormat, VkImage Image, VkImageAspectFlags ImageAspect)
	{
		VkImageViewCreateInfo ImageViewCreateInfo{};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext = nullptr;
		ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.image = Image;
		ImageViewCreateInfo.format = ImageFormat;
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = 1;
		ImageViewCreateInfo.subresourceRange.aspectMask = ImageAspect;

		return ImageViewCreateInfo;
	}

	void TransitionImage(VkCommandBuffer Cmd, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NewLayout)
	{
		VkImageSubresourceRange ImageSubresourceRange = CreateImageSubresourceRange(
			(NewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? 
			VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

		VkImageMemoryBarrier2 ImageMemoryBarrier{};
		ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		ImageMemoryBarrier.pNext = nullptr;
		ImageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		ImageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
		ImageMemoryBarrier.oldLayout = CurrentLayout;
		ImageMemoryBarrier.newLayout = NewLayout;
		ImageMemoryBarrier.image = Image;
		ImageMemoryBarrier.subresourceRange = ImageSubresourceRange;

		VkDependencyInfo DependencyInfo{};
		DependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		DependencyInfo.pNext = nullptr;
		DependencyInfo.imageMemoryBarrierCount = 1;
		DependencyInfo.pImageMemoryBarriers = &ImageMemoryBarrier;

		vkCmdPipelineBarrier2(Cmd, &DependencyInfo);
	}

	void CopyImageToImage(VkCommandBuffer Cmd, VkImage SrcImage, VkImage DstImage, VkExtent2D SrcSize, VkExtent2D DstSize)
	{
		VkImageBlit2 BlitRegion{};
		BlitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
		BlitRegion.pNext = nullptr;

		BlitRegion.srcOffsets[1].x = SrcSize.width;
		BlitRegion.srcOffsets[1].y = SrcSize.height;
		BlitRegion.srcOffsets[1].z = 1;

		BlitRegion.dstOffsets[1].x = DstSize.width;
		BlitRegion.dstOffsets[1].y = DstSize.height;
		BlitRegion.dstOffsets[1].z = 1;

		BlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		BlitRegion.srcSubresource.baseArrayLayer = 0;
		BlitRegion.srcSubresource.layerCount = 1;
		BlitRegion.srcSubresource.mipLevel = 0;

		BlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		BlitRegion.dstSubresource.baseArrayLayer = 0;
		BlitRegion.dstSubresource.layerCount = 1;
		BlitRegion.dstSubresource.mipLevel = 0;

		VkBlitImageInfo2 BlitImageInfo{};
		BlitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
		BlitImageInfo.pNext = nullptr;
		BlitImageInfo.dstImage = DstImage;
		BlitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		BlitImageInfo.srcImage = SrcImage;
		BlitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		BlitImageInfo.filter = VK_FILTER_LINEAR;
		BlitImageInfo.regionCount = 1;
		BlitImageInfo.pRegions = &BlitRegion;

		vkCmdBlitImage2(Cmd, &BlitImageInfo);
	}

	bool LoadShaderModule(const char* Path, VkDevice Device, VkShaderModule* ShaderModule)
	{
		std::ifstream FileHandle(Path, std::ios::ate | std::ios::binary);

		if (!FileHandle.is_open())
		{
			SAPPHIRE_ERROR("Failed to load Shader: {}", Path);

			return false;
		}

		size_t FileSize = (size_t)FileHandle.tellg();

		std::vector<uint32_t> FileBuffer(FileSize / sizeof(uint32_t));

		FileHandle.seekg(0);
		FileHandle.read((char*)FileBuffer.data(), FileSize);
		FileHandle.close();

		VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
		ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ShaderModuleCreateInfo.pNext = nullptr;
		ShaderModuleCreateInfo.codeSize = FileBuffer.size() * sizeof(uint32_t);
		ShaderModuleCreateInfo.pCode = FileBuffer.data();
		
		VkCheckResult(vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, ShaderModule));

		return false;
	}

	#ifdef SAPPHIRE_RENDER_DEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		const char* Message = pCallbackData->pMessage;

		switch (MessageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				SAPPHIRE_INFO(Message);
				break;
			}

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			{
				SAPPHIRE_WARN(Message);
				break;
			}

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			{
				SAPPHIRE_ERROR(Message);
				break;
			}

			default:
			{
				break;
			}
		}

		return VK_FALSE;
	}
	VkResult vkCreateDebugUtilsMessenger(VkInstance Instance,
		const VkDebugUtilsMessengerCreateInfoEXT* DebugUtilsMessengerCreateInfo, 
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		static auto Function = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");

		return Function(Instance, DebugUtilsMessengerCreateInfo, pAllocator, pDebugMessenger);
	}

	void vkDestroyDebugUtilsMessenger(VkInstance Instance, const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT DebugMessenger)
	{
		static auto Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");

		Function(Instance, DebugMessenger, pAllocator);
	}
	#endif
}