#include "Renderer.h"

#include "VkUtil.h"
#include "Pipeline.h"
#include "Loader.h"

#include "core/Application.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		CreateSwapchain();

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

		m_DeletionQueue.PushFunction([&]()
			{
				m_DescriptorAllocator.DestroyDescriptorPool(m_Device);
				vkDestroyDescriptorSetLayout(m_Device, m_DrawImageDescriptorSetLayout, nullptr);
			});

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

		VkPushConstantRange GradientPushConstantRange{};
		GradientPushConstantRange.offset = 0;
		GradientPushConstantRange.size = sizeof(GradientPushConstants);
		GradientPushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo ComputePipelineLayoutCreateInfo{};
		ComputePipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ComputePipelineLayoutCreateInfo.pNext = nullptr;
		ComputePipelineLayoutCreateInfo.pSetLayouts = &m_DrawImageDescriptorSetLayout;
		ComputePipelineLayoutCreateInfo.setLayoutCount = 1;
		ComputePipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		ComputePipelineLayoutCreateInfo.pPushConstantRanges = &GradientPushConstantRange;

		VkCheckResult(vkCreatePipelineLayout(m_Device, &ComputePipelineLayoutCreateInfo, nullptr, &m_GradientPipelineLayout));

		VkShaderModule GradientComputeShaderModule;
		LoadShaderModule("Shaders/gradient.comp.spv", m_Device, &GradientComputeShaderModule);

		ComputePipelineBuilder ComputeBuilder{};
		ComputeBuilder.SetShader(GradientComputeShaderModule);
		ComputeBuilder.PipelineLayout = m_GradientPipelineLayout;

		m_GradientPipeline = ComputeBuilder.BuildPipeline(m_Device);

		vkDestroyShaderModule(m_Device, GradientComputeShaderModule, nullptr);

		m_DeletionQueue.PushFunction([&]()
			{
				vkDestroyPipelineLayout(m_Device, m_GradientPipelineLayout, nullptr);
				vkDestroyPipeline(m_Device, m_GradientPipeline, nullptr);
			});

		VkPushConstantRange DrawPushConstantRange{};
		DrawPushConstantRange.offset = 0;
		DrawPushConstantRange.size = sizeof(GpuDrawPushConstants);
		DrawPushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkShaderModule TriangleVertexShaderModule;
		LoadShaderModule("Shaders/triangle.vert.spv", m_Device, &TriangleVertexShaderModule);
		VkShaderModule TriangleFragmentShaderModule;
		LoadShaderModule("Shaders/triangle.frag.spv", m_Device, &TriangleFragmentShaderModule);

		VkPipelineLayoutCreateInfo TrianglePipelineLayoutCreateInfo{};
		TrianglePipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		TrianglePipelineLayoutCreateInfo.pNext = nullptr;
		TrianglePipelineLayoutCreateInfo.pSetLayouts = nullptr;
		TrianglePipelineLayoutCreateInfo.setLayoutCount = 0;
		TrianglePipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		TrianglePipelineLayoutCreateInfo.pPushConstantRanges = &DrawPushConstantRange;

		VkCheckResult(vkCreatePipelineLayout(m_Device, &TrianglePipelineLayoutCreateInfo, nullptr, &m_TrianglePipelineLayout));

		GraphicsPipelineBuilder GraphicsPipelineBuilder;

		GraphicsPipelineBuilder.Clear();

		GraphicsPipelineBuilder.PipelineLayout = m_TrianglePipelineLayout;
		GraphicsPipelineBuilder.SetShaders(TriangleVertexShaderModule, TriangleFragmentShaderModule);
		GraphicsPipelineBuilder.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		GraphicsPipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
		GraphicsPipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
		GraphicsPipelineBuilder.DisableMultisampling();
		GraphicsPipelineBuilder.DisableBlending();
		GraphicsPipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
		GraphicsPipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
		GraphicsPipelineBuilder.SetDepthFormat(m_DepthImage.ImageFormat);

		m_TrianglePipeline = GraphicsPipelineBuilder.BuildPipeline(m_Device);

		vkDestroyShaderModule(m_Device, TriangleVertexShaderModule, nullptr);
		vkDestroyShaderModule(m_Device, TriangleFragmentShaderModule, nullptr);

		m_DeletionQueue.PushFunction([&]()
			{
				vkDestroyPipelineLayout(m_Device, m_TrianglePipelineLayout, nullptr);
				vkDestroyPipeline(m_Device, m_TrianglePipeline, nullptr);
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

		SwapchainInfo SwapchainInfo = GetSwapchainInfo(m_PhysicalDevice, m_Surface);

		VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(SwapchainInfo.SurfaceFormats);
		uint32_t SwapchainImageCount = SwapchainInfo.SurfaceCapabilities.minImageCount + 1;

		ImGui_ImplVulkan_InitInfo ImGuiVulkanInitInfo{};
		ImGuiVulkanInitInfo.Instance = m_Instance;
		ImGuiVulkanInitInfo.PhysicalDevice = m_PhysicalDevice;
		ImGuiVulkanInitInfo.Device = m_Device;
		ImGuiVulkanInitInfo.Queue = m_GraphicsQueue;
		ImGuiVulkanInitInfo.DescriptorPool = ImGuiDescriptorPool;
		ImGuiVulkanInitInfo.MinImageCount = SwapchainImageCount;
		ImGuiVulkanInitInfo.ImageCount = SwapchainImageCount;
		ImGuiVulkanInitInfo.UseDynamicRendering = true;
		ImGuiVulkanInitInfo.ColorAttachmentFormat = SurfaceFormat.format;
		ImGuiVulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&ImGuiVulkanInitInfo, VK_NULL_HANDLE);

		m_ImmediateContext.SubmitImmediate(m_Device, [&](VkCommandBuffer Cmd)
			{
				ImGui_ImplVulkan_CreateFontsTexture(Cmd);
			});

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		m_DeletionQueue.PushFunction([=]()
			{
				vkDestroyDescriptorPool(m_Device, ImGuiDescriptorPool, nullptr);
				ImGui_ImplVulkan_Shutdown();
			});

		m_TestMeshes = LoadGLTFMesh(this, "Models/basicmesh.glb").value();

		m_DeletionQueue.PushFunction([=]()
			{
				for (const auto& Mesh : m_TestMeshes)
				{
					DestroyBuffer(Mesh->Meshbuffer.IndexBuffer);
					DestroyBuffer(Mesh->Meshbuffer.VertexBuffer);
				}
			});
	}
	
	glm::vec3 Position{ 0, 0, -3 };
	glm::vec3 Rotation{};

	void Renderer::Draw()
	{
		if (ResizeRequested)
		{
			ResizeSwapchain();
		}

		ImGui::Begin("Model Data");
		ImGui::InputFloat3("Position", glm::value_ptr(Position));
		ImGui::InputFloat3("Rotation", glm::value_ptr(Rotation));
		ImGui::End();

		ImGui_ImplVulkan_NewFrame();

		ImGui::Render();

		VkCheckResult(vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, true, UINT64_MAX));
		VkCheckResult(vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence));

		uint32_t SwapchainImageIndex;
		VkResult Result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, GetCurrentFrame().PresentSemaphore,
			nullptr, &SwapchainImageIndex);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ResizeRequested = true;
			return;
		}

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

		GradientPushConstants PushConstants{};
		PushConstants.data1 = glm::vec4(1, 0, 0, 1);
		PushConstants.data2 = glm::vec4(0, 0, 1, 1);

		vkCmdPushConstants(Cmd, m_GradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(GradientPushConstants), &PushConstants);

		vkCmdDispatch(Cmd, (uint32_t)std::ceil(m_DrawExtent.width / 16.0), (uint32_t)std::ceil(m_DrawExtent.height / 16.0), 1);

		TransitionImage(Cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		TransitionImage(Cmd, m_DepthImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo GraphicsPipelineColorRenderingAttachment = ColorAttachmentInfo(m_DrawImage.ImageView, nullptr,
			VK_IMAGE_LAYOUT_GENERAL);
		VkRenderingAttachmentInfo GraphicsPipelineDepthRenderingAttachment = DepthAttachmentInfo(m_DepthImage.ImageView,
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		VkRenderingInfo GraphicsPipelineRenderingInfo{};
		GraphicsPipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		GraphicsPipelineRenderingInfo.pNext = nullptr;
		GraphicsPipelineRenderingInfo.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, m_DrawExtent };
		GraphicsPipelineRenderingInfo.layerCount = 1;
		GraphicsPipelineRenderingInfo.colorAttachmentCount = 1;
		GraphicsPipelineRenderingInfo.pColorAttachments = &GraphicsPipelineColorRenderingAttachment;
		GraphicsPipelineRenderingInfo.pDepthAttachment = &GraphicsPipelineDepthRenderingAttachment;
		GraphicsPipelineRenderingInfo.pStencilAttachment = nullptr; 

		vkCmdBeginRendering(Cmd, &GraphicsPipelineRenderingInfo);

		VkViewport Viewport{};
		Viewport.x = 0;
		Viewport.y = 0;
		Viewport.width = (float)m_DrawExtent.width;
		Viewport.height = (float)m_DrawExtent.height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;

		vkCmdSetViewport(Cmd, 0, 1, &Viewport);

		VkRect2D Scissor{};
		Scissor.offset.x = 0;
		Scissor.offset.y = 0;
		Scissor.extent.width = m_DrawExtent.width;
		Scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(Cmd, 0, 1, &Scissor);

		glm::mat4 view(1.0);
		view = glm::translate(view, Position);
		view = glm::rotate(view, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
		view = glm::rotate(view, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
		view = glm::rotate(view, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
		glm::mat4 proj = glm::perspective(glm::radians(70.f), float(m_DrawExtent.width) / m_DrawExtent.height, 0.1f, 10000.0f);

		//invert Y-Axis for compatibility with glsl/gltf
		proj[1][1] *= -1;

		GpuDrawPushConstants DrawPushConst{};
		DrawPushConst.WorldMatrix = proj * view;
		DrawPushConst.VertexBuffer = m_TestMeshes[2]->Meshbuffer.VertexBufferAddress;

		vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);

		vkCmdPushConstants(Cmd, m_TrianglePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GpuDrawPushConstants), &DrawPushConst);

		vkCmdBindIndexBuffer(Cmd, m_TestMeshes[2]->Meshbuffer.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(Cmd, m_TestMeshes[2]->Surfaces[0].Count, 1, m_TestMeshes[2]->Surfaces[0].StartIndex, 0, 0);

		vkCmdEndRendering(Cmd);

		TransitionImage(Cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		TransitionImage(Cmd, m_SwapchainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyImageToImage(Cmd, m_DrawImage.Image, m_SwapchainImages[SwapchainImageIndex], m_DrawExtent, m_SwapchainExtent);

		TransitionImage(Cmd, m_SwapchainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo RenderingColorAttachmentInfo = ColorAttachmentInfo(m_SwapchainImageViews[SwapchainImageIndex], nullptr,
			VK_IMAGE_LAYOUT_GENERAL);

		VkRenderingInfo ImGuiRenderingInfo{};
		ImGuiRenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		ImGuiRenderingInfo.pNext = nullptr;
		ImGuiRenderingInfo.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, m_SwapchainExtent };
		ImGuiRenderingInfo.layerCount = 1;
		ImGuiRenderingInfo.colorAttachmentCount = 1;
		ImGuiRenderingInfo.pColorAttachments = &RenderingColorAttachmentInfo;
		ImGuiRenderingInfo.pDepthAttachment = nullptr;
		ImGuiRenderingInfo.pStencilAttachment = nullptr;	

		vkCmdBeginRendering(Cmd, &ImGuiRenderingInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd);

		vkCmdEndRendering(Cmd);

		TransitionImage(Cmd, m_SwapchainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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

		Result = vkQueuePresentKHR(m_GraphicsQueue, &PresentInfo);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ResizeRequested = true;
		}

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

		vkDestroyImageView(m_Device, m_DepthImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DepthImage.Image, m_DepthImage.Allocation);

		vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
		
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

		vmaDestroyAllocator(m_Allocator);

		vkDestroyDevice(m_Device, nullptr);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		#ifdef SAPPHIRE_RENDER_DEBUG
		vkDestroyDebugUtilsMessenger(m_Instance, nullptr, m_DebugMessenger);
		#endif

		vkDestroyInstance(m_Instance, nullptr);
	}

	AllocatedBuffer Renderer::CreateBuffer(size_t BufferSize, VkBufferUsageFlags BufferUsage, VmaMemoryUsage BufferMemoryUsage)
	{
		VkBufferCreateInfo BufferCreateInfo{};
		BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCreateInfo.pNext = nullptr;
		BufferCreateInfo.size = BufferSize;
		BufferCreateInfo.usage = BufferUsage;

		VmaAllocationCreateInfo AllocationCreateInfo{};
		AllocationCreateInfo.usage = BufferMemoryUsage;
		AllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		AllocatedBuffer Buffer{};

		VkCheckResult(vmaCreateBuffer(
			m_Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer.Buffer, &Buffer.Allocation, &Buffer.AllocationInfo));

		return Buffer;
	}

	void Renderer::DestroyBuffer(const AllocatedBuffer& Buffer)
	{
		vmaDestroyBuffer(m_Allocator, Buffer.Buffer, Buffer.Allocation);
	}

	void Renderer::CreateSwapchain()
	{
		QueueFamilyIndices QueueFamilies = FindQueueFamilies(m_PhysicalDevice, m_Surface);

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

		m_DepthImage.ImageFormat = VK_FORMAT_D32_SFLOAT;
		m_DepthImage.ImageExtent = DrawImageExtent;

		VkImageUsageFlags DepthImageUsageFlags{};
		DepthImageUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		VkImageCreateInfo DepthImageCreateInfo = CreateImageCreateInfo(m_DepthImage.ImageFormat, DepthImageUsageFlags,
			DrawImageExtent);

		VmaAllocationCreateInfo DepthImageAllocationCreateInfo{};
		DepthImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		DepthImageAllocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &DepthImageCreateInfo, &DepthImageAllocationCreateInfo, &m_DepthImage.Image,
			&m_DepthImage.Allocation, nullptr);

		VkImageViewCreateInfo DepthImageViewCreateInfo = CreateImageViewCreateInfo(m_DepthImage.ImageFormat,
			m_DepthImage.Image, VK_IMAGE_ASPECT_DEPTH_BIT);

		VkCheckResult(vkCreateImageView(m_Device, &DepthImageViewCreateInfo, nullptr, &m_DepthImage.ImageView));
	}

	void Renderer::ResizeSwapchain()
	{
		vkDestroyImageView(m_Device, m_DepthImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DepthImage.Image, m_DepthImage.Allocation);

		vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);

		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

		CreateSwapchain();

		ResizeRequested = false;
	}

	GpuMeshBuffer Renderer::UploadMesh(std::span<Vertex> Vertices, std::span<uint32_t> Indices)
	{
		const size_t VBSize = Vertices.size() * sizeof(Vertex);
		const size_t IBSize = Indices.size() * sizeof(uint32_t);

		GpuMeshBuffer MeshBuffer{};

		MeshBuffer.VertexBuffer = CreateBuffer(VBSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT 
			| VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		MeshBuffer.IndexBuffer = CreateBuffer(IBSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY);

		VkBufferDeviceAddressInfo DeviceAddressInfo{};
		DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		DeviceAddressInfo.buffer = MeshBuffer.VertexBuffer.Buffer;

		MeshBuffer.VertexBufferAddress = vkGetBufferDeviceAddress(m_Device, &DeviceAddressInfo);

		AllocatedBuffer StagingBuffer = CreateBuffer(VBSize + IBSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* Data = StagingBuffer.Allocation->GetMappedData();
		memcpy(Data, Vertices.data(), VBSize);
		memcpy((char*)Data + VBSize, Indices.data(), IBSize);

		m_ImmediateContext.SubmitImmediate(m_Device, [&](VkCommandBuffer Cmd)
			{
				VkBufferCopy VertexBufferCopy{};
				VertexBufferCopy.dstOffset = 0;
				VertexBufferCopy.srcOffset = 0;
				VertexBufferCopy.size = VBSize;

				vkCmdCopyBuffer(Cmd, StagingBuffer.Buffer, MeshBuffer.VertexBuffer.Buffer, 1, &VertexBufferCopy);

				VkBufferCopy IndexBufferCopy{};
				IndexBufferCopy.dstOffset = 0;
				IndexBufferCopy.srcOffset = VBSize;
				IndexBufferCopy.size = IBSize;

				vkCmdCopyBuffer(Cmd, StagingBuffer.Buffer, MeshBuffer.IndexBuffer.Buffer, 1, &IndexBufferCopy);
			});

		DestroyBuffer(StagingBuffer);

		return MeshBuffer;
	}
}