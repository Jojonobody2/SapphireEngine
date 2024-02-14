#include "Renderer.h"

#include "VkUtil.h"

#include "core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include "core/imgui_impl_glfw.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <set>

namespace Sapphire
{
	void Renderer::Init()
	{
		VkApplicationInfo AppInfo{};
		AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext = nullptr;
		AppInfo.pApplicationName = Application::Get().GetName();
		AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.pEngineName = "Sapphire Engine";
		AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		AppInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

		std::vector<const char*> InstanceExtensions = GetRequiredInstanceExtensions();
		std::vector<const char*> InstanceLayers{};
		#ifdef SAPPHIRE_RENDER_DEBUG
		InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (IsLayerSupported("VK_LAYER_KHRONOS_validation"))
			InstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
		else
			SAPPHIRE_ERROR("Render Validation requested but VK_LAYER_KHRONOS_validation not available!");
		#endif

		VkInstanceCreateInfo InstanceCreateInfo{};
		InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstanceCreateInfo.pNext = nullptr;
		InstanceCreateInfo.flags = 0;
		InstanceCreateInfo.pApplicationInfo = &AppInfo;
		InstanceCreateInfo.enabledLayerCount = (uint32_t)InstanceLayers.size();
		InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.data();
		InstanceCreateInfo.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
		InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

		VkCheckResult(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance));

		#ifdef SAPPHIRE_RENDER_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo{};
		DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugUtilsMessengerCreateInfo.pNext = nullptr;
		DebugUtilsMessengerCreateInfo.flags = 0;
		DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
		DebugUtilsMessengerCreateInfo.pUserData = nullptr;

		VkCheckResult(vkCreateDebugUtilsMessenger(m_Instance, &DebugUtilsMessengerCreateInfo, 
			nullptr, &m_DebugMessenger));
		#endif

		m_Surface = CreateSurface(m_Instance);

		m_PhysicalDevice = SelectPhysicalDevice(m_Instance, m_Surface);

		QueueFamilyIndices QueueFamilies = FindQueueFamilies(m_PhysicalDevice, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos{};
		std::set<uint32_t> UniqueQueueFamilies{
			QueueFamilies.GraphicsFamily.value(), QueueFamilies.PresentFamily.value() };

		float QueuePriority = 1.0f;

		VkDeviceQueueCreateInfo QueueCreateInfo{};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.pNext = nullptr;
		QueueCreateInfo.flags = 0;
		QueueCreateInfo.queueCount = 1;
		QueueCreateInfo.pQueuePriorities = &QueuePriority;

		for (uint32_t QueueFamily : UniqueQueueFamilies)
		{
			QueueCreateInfo.queueFamilyIndex = QueueFamily;
			QueueCreateInfos.push_back(QueueCreateInfo);
		}

		std::vector<const char*> DeviceExtensions{};
		DeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		VkPhysicalDeviceVulkan12Features DeviceFeatures12{};
		DeviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		DeviceFeatures12.bufferDeviceAddress = true;
		DeviceFeatures12.descriptorIndexing = true;

		VkPhysicalDeviceVulkan13Features DeviceFeatures13{};
		DeviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		DeviceFeatures13.pNext = &DeviceFeatures12;
		DeviceFeatures13.dynamicRendering = true;
		DeviceFeatures13.synchronization2 = true;
		
		VkDeviceCreateInfo DeviceCreateInfo{};
		DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceCreateInfo.pNext = &DeviceFeatures13;
		DeviceCreateInfo.flags = 0;
		DeviceCreateInfo.queueCreateInfoCount = (uint32_t)QueueCreateInfos.size();
		DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
		#ifdef SAPPHIRE_RENDER_DEBUG
		const char* ValidationLayer = "VK_LAYER_KHRONOS_validation";
		DeviceCreateInfo.enabledLayerCount = 1;
		DeviceCreateInfo.ppEnabledLayerNames = &ValidationLayer;
		#else
		DeviceCreateInfo.enabledLayerCount = 0;
		DeviceCreateInfo.ppEnabledLayerNames = nullptr;
		#endif
		DeviceCreateInfo.enabledExtensionCount = (uint32_t)DeviceExtensions.size();
		DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

		VkCheckResult(vkCreateDevice(m_PhysicalDevice, &DeviceCreateInfo, nullptr, &m_Device));

		vkGetDeviceQueue(m_Device, QueueFamilies.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, QueueFamilies.PresentFamily.value(), 0, &m_PresentQueue);

		VmaAllocatorCreateInfo AllocatorCreateInfo{};
		AllocatorCreateInfo.physicalDevice = m_PhysicalDevice;
		AllocatorCreateInfo.device = m_Device;
		AllocatorCreateInfo.instance = m_Instance;
		AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		VkCheckResult(vmaCreateAllocator(&AllocatorCreateInfo, &m_Allocator));

		m_DeletionQueue.PushFunction([&]() { vmaDestroyAllocator(m_Allocator); });

		SwapchainInfo SwapchainInfo = GetSwapchainInfo(m_PhysicalDevice, m_Surface);

		VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(SwapchainInfo.SurfaceFormats);
		VkPresentModeKHR PresentMode = ChoosePresentMode(SwapchainInfo.PresentModes);
		m_SwapchainExtent = ChooseSwapchainExtent(SwapchainInfo.SurfaceCapabilities);

		uint32_t SwapchainImageCount = SwapchainInfo.SurfaceCapabilities.minImageCount + 1;

		if (SwapchainInfo.SurfaceCapabilities.maxImageCount > 0 && 
			SwapchainInfo.SurfaceCapabilities.maxImageCount < SwapchainImageCount)
		{
			SwapchainImageCount = SwapchainInfo.SurfaceCapabilities.maxImageCount;
		}

		uint32_t QueueFamiliesA[] = { QueueFamilies.GraphicsFamily.value(), QueueFamilies.PresentFamily.value() };

		VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
		SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainCreateInfo.pNext = nullptr;
		SwapchainCreateInfo.flags = 0;
		SwapchainCreateInfo.surface = m_Surface;
		SwapchainCreateInfo.minImageCount = SwapchainImageCount;
		SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
		SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
		SwapchainCreateInfo.imageExtent = m_SwapchainExtent;
		SwapchainCreateInfo.imageArrayLayers = 1;
		SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (QueueFamilies.GraphicsFamily.value() != QueueFamilies.GraphicsFamily.value())
		{
			SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			SwapchainCreateInfo.queueFamilyIndexCount = 2;
			SwapchainCreateInfo.pQueueFamilyIndices = QueueFamiliesA;
		}
		else
		{
			SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			SwapchainCreateInfo.queueFamilyIndexCount = 0;
			SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		SwapchainCreateInfo.preTransform = SwapchainInfo.SurfaceCapabilities.currentTransform;
		SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		SwapchainCreateInfo.presentMode = PresentMode;
		SwapchainCreateInfo.clipped = VK_TRUE;
		SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkCheckResult(vkCreateSwapchainKHR(m_Device, &SwapchainCreateInfo, nullptr, &m_Swapchain));

		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, nullptr);
		m_SwapchainImages.resize(SwapchainImageCount);
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &SwapchainImageCount, m_SwapchainImages.data());

		m_SwapchainImageViews.resize(SwapchainImageCount);

		VkImageViewCreateInfo SwapchainImageViewCreateInfo{};
		SwapchainImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		SwapchainImageViewCreateInfo.pNext = nullptr;
		SwapchainImageViewCreateInfo.flags = 0;
		SwapchainImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		SwapchainImageViewCreateInfo.format = SurfaceFormat.format;
		SwapchainImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		SwapchainImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		SwapchainImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		SwapchainImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		SwapchainImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		SwapchainImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		SwapchainImageViewCreateInfo.subresourceRange.levelCount = 1;
		SwapchainImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		SwapchainImageViewCreateInfo.subresourceRange.layerCount = 1;

		for (uint32_t i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			SwapchainImageViewCreateInfo.image = m_SwapchainImages[i];

			VkCheckResult(vkCreateImageView(m_Device, &SwapchainImageViewCreateInfo, nullptr, 
				&m_SwapchainImageViews[i]));
		}

		VkExtent3D DrawImageExtent =
		{
			m_SwapchainExtent.width,
			m_SwapchainExtent.height,
			1
		};

		m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.ImageExtent = DrawImageExtent;

		VkImageUsageFlags DrawImageUsageFlags{};
		DrawImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		DrawImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		DrawImageUsageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		DrawImageUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo DrawImageCreateInfo = CreateImageCreateInfo(m_DrawImage.ImageFormat, DrawImageUsageFlags,
			DrawImageExtent);

		VmaAllocationCreateInfo DrawImageAllocationCreateInfo{};
		DrawImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		DrawImageAllocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &DrawImageCreateInfo, &DrawImageAllocationCreateInfo, &m_DrawImage.Image,
			&m_DrawImage.Allocation, nullptr);

		VkImageViewCreateInfo DrawImageViewCreateInfo = CreateImageViewCreateInfo(m_DrawImage.ImageFormat,
			m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

		VkCheckResult(vkCreateImageView(m_Device, &DrawImageViewCreateInfo, nullptr, &m_DrawImage.ImageView));

		m_DeletionQueue.PushFunction([&] 
			{
				vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
				vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
			});

		VkCommandPoolCreateInfo CommandPoolCreateInfo{};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = QueueFamilies.GraphicsFamily.value();

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = nullptr;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;

		for (uint32_t i = 0; i < FRAMES_IN_FLIGTH; i++)
		{
			VkCheckResult(vkCreateCommandPool(m_Device, &CommandPoolCreateInfo, nullptr, &m_FrameData[i].CommandPool));
		
			CommandBufferAllocateInfo.commandPool = m_FrameData[i].CommandPool;

			VkCheckResult(vkAllocateCommandBuffers(m_Device, &CommandBufferAllocateInfo, &m_FrameData[i].CommandBuffer));
		}

		VkFenceCreateInfo FenceCreateInfo{};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo SemaphoreCreateInfo{};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.pNext = nullptr;
		SemaphoreCreateInfo.flags = 0;

		for (uint32_t i = 0; i < FRAMES_IN_FLIGTH; i++)
		{
			VkCheckResult(vkCreateFence(m_Device, &FenceCreateInfo, nullptr, &m_FrameData[i].RenderFence));

			VkCheckResult(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_FrameData[i].PresentSemaphore));
			VkCheckResult(vkCreateSemaphore(m_Device, &SemaphoreCreateInfo, nullptr, &m_FrameData[i].RenderSemaphore));
		}

		m_ImmediateContext.CreateImmediateContext(m_Device, QueueFamilies.GraphicsFamily.value(), m_GraphicsQueue);
		m_DeletionQueue.PushFunction([&]()
			{
				m_ImmediateContext.DestroyImmediateContext(m_Device);
			});

		std::vector<DescriptorAllocator::PoolSizeRatio> Sizes =
		{
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
		};

		m_DescriptorAllocator.InitDescriptorPool(m_Device, 10, Sizes);

		{
			DescriptorLayoutBuilder Builder;
			Builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			m_DrawImageDescriptorSetLayout = Builder.Build(m_Device, VK_SHADER_STAGE_COMPUTE_BIT);
		}

		m_DrawImageDescriptor = m_DescriptorAllocator.AllocateSet(m_Device, m_DrawImageDescriptorSetLayout);

		VkDescriptorImageInfo ImageInfo{};
		ImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		ImageInfo.imageView = m_DrawImage.ImageView;

		VkWriteDescriptorSet DrawImageWrite{};
		DrawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DrawImageWrite.pNext = nullptr;
		DrawImageWrite.dstBinding = 0;
		DrawImageWrite.dstSet = m_DrawImageDescriptor;
		DrawImageWrite.descriptorCount = 1;
		DrawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		DrawImageWrite.pImageInfo = &ImageInfo;

		vkUpdateDescriptorSets(m_Device, 1, &DrawImageWrite, 0, nullptr);

		VkPipelineLayoutCreateInfo ComputePipelineLayoutCreateInfo{};
		ComputePipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ComputePipelineLayoutCreateInfo.pNext = nullptr;
		ComputePipelineLayoutCreateInfo.pSetLayouts = &m_DrawImageDescriptorSetLayout;
		ComputePipelineLayoutCreateInfo.setLayoutCount = 1;

		VkCheckResult(vkCreatePipelineLayout(m_Device, &ComputePipelineLayoutCreateInfo, nullptr, &m_GradientPipelineLayout));

		VkShaderModule GradientComputeShaderModule;
		LoadShaderModule("Shaders/gradient.comp.spv", m_Device, &GradientComputeShaderModule);

		VkPipelineShaderStageCreateInfo GradientComputeShaderStageCreateInfo{};
		GradientComputeShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		GradientComputeShaderStageCreateInfo.pNext = nullptr;
		GradientComputeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		GradientComputeShaderStageCreateInfo.module = GradientComputeShaderModule;
		GradientComputeShaderStageCreateInfo.pName = "main";

		VkComputePipelineCreateInfo GradientComputePipelineCreateInfo{};
		GradientComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		GradientComputePipelineCreateInfo.pNext = nullptr;
		GradientComputePipelineCreateInfo.layout = m_GradientPipelineLayout;
		GradientComputePipelineCreateInfo.stage = GradientComputeShaderStageCreateInfo;

		VkCheckResult(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &GradientComputePipelineCreateInfo, nullptr,
			&m_GradientPipeline));

		vkDestroyShaderModule(m_Device, GradientComputeShaderModule, nullptr);

		m_DeletionQueue.PushFunction([&]()
			{
				vkDestroyPipelineLayout(m_Device, m_GradientPipelineLayout, nullptr);
				vkDestroyPipeline(m_Device, m_GradientPipeline, nullptr);
			});

		VkDescriptorPoolSize ImGuiDescriptorPoolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo{};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		DescriptorPoolCreateInfo.maxSets = 1000;
		DescriptorPoolCreateInfo.poolSizeCount = (uint32_t)std::size(ImGuiDescriptorPoolSizes);
		DescriptorPoolCreateInfo.pPoolSizes = ImGuiDescriptorPoolSizes;

		VkDescriptorPool ImGuiDescriptorPool;
		VkCheckResult(vkCreateDescriptorPool(m_Device, &DescriptorPoolCreateInfo, nullptr, &ImGuiDescriptorPool));

		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::Get().GetWindow()->GetNativeWindow(), true);

		ImGui_ImplVulkan_InitInfo ImGuiVulkanInitInfo{};
		ImGuiVulkanInitInfo.Instance = m_Instance;
		ImGuiVulkanInitInfo.PhysicalDevice = m_PhysicalDevice;
		ImGuiVulkanInitInfo.Device = m_Device;
		ImGuiVulkanInitInfo.Queue = m_GraphicsQueue;
		ImGuiVulkanInitInfo.DescriptorPool = ImGuiDescriptorPool;
		ImGuiVulkanInitInfo.MinImageCount = 3;
		ImGuiVulkanInitInfo.ImageCount = 3;
		ImGuiVulkanInitInfo.UseDynamicRendering = true;
		ImGuiVulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&ImGuiVulkanInitInfo, VK_NULL_HANDLE);

		ImGui_ImplVulkan_CreateFontsTexture();


	}
		
	void Renderer::Draw()
	{
		VkCheckResult(vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, true, UINT64_MAX));
		VkCheckResult(vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence));

		uint32_t SwapchainImageIndex;
		vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, GetCurrentFrame().PresentSemaphore, 
			nullptr, &SwapchainImageIndex);

		VkCommandBuffer Cmd = GetCurrentFrame().CommandBuffer;

		VkCheckResult(vkResetCommandBuffer(Cmd, 0));

		m_DrawExtent.width = m_DrawImage.ImageExtent.width;
		m_DrawExtent.height = m_DrawImage.ImageExtent.height;

		VkCommandBufferBeginInfo CmdBeginInfo{};
		CmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CmdBeginInfo.pNext = nullptr;
		CmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CmdBeginInfo.pInheritanceInfo = nullptr;

		VkCheckResult(vkBeginCommandBuffer(Cmd, &CmdBeginInfo));

		TransitionImage(Cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		VkClearColorValue ClearValue = { 1.0f, 0.0f, 1.0f, 1.0f };

		VkImageSubresourceRange ClearRange = CreateImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_GradientPipeline);

		vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_GradientPipelineLayout, 0, 1, &m_DrawImageDescriptor, 0, nullptr);

		vkCmdDispatch(Cmd, (uint32_t)std::ceil(m_DrawExtent.width / 16.0), (uint32_t)std::ceil(m_DrawExtent.height / 16.0), 1);

		TransitionImage(Cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionImage(Cmd, m_SwapchainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyImageToImage(Cmd, m_DrawImage.Image, m_SwapchainImages[SwapchainImageIndex], m_DrawExtent, m_SwapchainExtent);

		TransitionImage(Cmd, m_SwapchainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VkCheckResult(vkEndCommandBuffer(Cmd));

		VkCommandBufferSubmitInfo CmdSubmitInfo = CreateCmdSubmitInfo(Cmd);

		VkSemaphoreSubmitInfo SemaphoreWaitInfo = CreateSemaphoreSubmitInfo(
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().PresentSemaphore);
		VkSemaphoreSubmitInfo SemaphoreSignalInfo = CreateSemaphoreSubmitInfo(
			VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().RenderSemaphore);

		VkSubmitInfo2 SubmitInfo = CreateSubmitInfo(&CmdSubmitInfo, &SemaphoreSignalInfo, &SemaphoreWaitInfo);

		VkCheckResult(vkQueueSubmit2(m_GraphicsQueue, 1, &SubmitInfo, GetCurrentFrame().RenderFence));

		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.pNext = nullptr;
		PresentInfo.pSwapchains = &m_Swapchain;
		PresentInfo.swapchainCount = 1;
		PresentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pImageIndices = &SwapchainImageIndex;

		VkCheckResult(vkQueuePresentKHR(m_GraphicsQueue, &PresentInfo));

		m_FrameIndex = (m_FrameIndex + 1) % FRAMES_IN_FLIGTH;
	}

	void Renderer::Shutdown()
	{
		vkDeviceWaitIdle(m_Device);

		m_DeletionQueue.Flush();

		for (uint32_t i = 0; i < FRAMES_IN_FLIGTH; i++)
		{
			vkDestroyCommandPool(m_Device, m_FrameData[i].CommandPool, nullptr);

			vkDestroyFence(m_Device, m_FrameData[i].RenderFence, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].RenderSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_FrameData[i].PresentSemaphore, nullptr);

			m_FrameData[i].DeletionQueue.Flush();
		}

		for (const VkImageView SwapchainImageView : m_SwapchainImageViews)
		{
			vkDestroyImageView(m_Device, SwapchainImageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		#ifdef SAPPHIRE_RENDER_DEBUG
		vkDestroyDebugUtilsMessenger(m_Instance, nullptr, m_DebugMessenger);
		#endif

		vkDestroyInstance(m_Instance, nullptr);
	}
}