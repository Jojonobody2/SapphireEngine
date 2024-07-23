#include "Renderer.h"

#include "Pipeline.h"
#include "VulkanUtil.h"
#include "VulkanStructs.h"

#include "Sapphire/Logging/Logger.h"
#include "Sapphire/Import/Model/ModelImporter.h"

#include <glm/glm.hpp>

#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

namespace Sapphire
{
    Renderer::Renderer()
    {
        SAPPHIRE_ENGINE_INFO("Initializing Rendering Engine!");

        m_RenderContext = CreateSharedPtr<RenderContext>();
        m_Swapchain = CreateSharedPtr<Swapchain>(m_RenderContext, FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_CommandLists.push_back(CreateSharedPtr<CommandList>(m_RenderContext));
        }

        m_GPUMemoryAllocator = CreateSharedPtr<GPUMemoryAllocator>(m_RenderContext);

        m_RenderImage = m_GPUMemoryAllocator->AllocateImage(m_Swapchain->GetExtent(), VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));

        m_DepthImage = m_GPUMemoryAllocator->AllocateImage(m_Swapchain->GetExtent(), VK_FORMAT_D32_SFLOAT,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT));

        InitImGui();

        SharedPtr<Shader> VertexShader = CreateSharedPtr<Shader>("Resources/Shaders/Shader.vert.spv", m_RenderContext);
        SharedPtr<Shader> FragmentShader = CreateSharedPtr<Shader>("Resources/Shaders/Shader.frag.spv", m_RenderContext);

        DescriptorSetLayoutBuilder Builder{};
        Builder.AddUBO(0, VK_SHADER_STAGE_VERTEX_BIT);

        m_GraphicsSetLayout = Builder.Build(m_RenderContext);

        GraphicsPipelineInfo TrianglePipelineInfo{};
        TrianglePipelineInfo.VertexShader = VertexShader;
        TrianglePipelineInfo.FragmentShader = FragmentShader;
        TrianglePipelineInfo.ColorAttachmentCount = 1;
        TrianglePipelineInfo.pColorAttachments = &m_RenderImage.ImageFormat;
        TrianglePipelineInfo.pDepthAttachmentFormat = &m_DepthImage.ImageFormat;
        TrianglePipelineInfo.Wireframe = false;
        TrianglePipelineInfo.DescriptorCount = 1;
        TrianglePipelineInfo.pDescriptors = &m_GraphicsSetLayout;

        m_GraphicsPipeline = CreateSharedPtr<GraphicsPipeline>(m_RenderContext, TrianglePipelineInfo);
        m_DescriptorAllocator = CreateSharedPtr<DescriptorAllocator>(m_RenderContext);

        MeshData MonkeyMeshData = ModelImporter::Import("Resources/Models/monkey.glb");
        m_MonkeyMesh = m_GPUMemoryAllocator->UploadMesh(MonkeyMeshData);

        m_GraphicsDescriptor = m_DescriptorAllocator->AllocateSet(m_GraphicsSetLayout);

        m_GraphicsUBO = m_GPUMemoryAllocator->AllocateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(GraphicsDescriptor));

        DescriptorWriter Writer{};
        Writer.AddBuffer(0, m_GraphicsUBO);
        Writer.WriteSet(m_RenderContext, m_GraphicsDescriptor);
    }

    Renderer::~Renderer()
    {
        VkCheck(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

        vkDestroyDescriptorSetLayout(m_RenderContext->GetDevice(), m_GraphicsSetLayout, nullptr);

        m_GPUMemoryAllocator->DestroyBuffer(m_GraphicsUBO);
        
        m_GPUMemoryAllocator->DestroyMesh(m_MonkeyMesh);

        m_GPUMemoryAllocator->DestroyImage(m_DepthImage);
        m_GPUMemoryAllocator->DestroyImage(m_RenderImage);

        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(m_RenderContext->GetDevice(), m_ImGuiPool, nullptr);

        SAPPHIRE_ENGINE_INFO("Shutting Rendering Engine down!");
    }

    void Renderer::InitImGui()
    {
        VkDescriptorPoolSize PoolSizes[] =
        { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
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
        DescriptorPoolCreateInfo.poolSizeCount = (uint32_t)std::size(PoolSizes);
        DescriptorPoolCreateInfo.pPoolSizes = PoolSizes;

        VkCheck(vkCreateDescriptorPool(m_RenderContext->GetDevice(), &DescriptorPoolCreateInfo,
                                       nullptr, &m_ImGuiPool));

        ImGui_ImplVulkan_InitInfo ImGuiInitInfo{};
        ImGuiInitInfo.Instance = m_RenderContext->GetInstance();
        ImGuiInitInfo.PhysicalDevice = m_RenderContext->GetPhysicalDevice();
        ImGuiInitInfo.Device = m_RenderContext->GetDevice();
        ImGuiInitInfo.Queue = m_RenderContext->GetGraphicsQueue().DeviceQueue;
        ImGuiInitInfo.DescriptorPool = m_ImGuiPool;
        ImGuiInitInfo.MinImageCount = m_Swapchain->GetImageCount();
        ImGuiInitInfo.ImageCount = m_Swapchain->GetImageCount();
        ImGuiInitInfo.UseDynamicRendering = true;
        ImGuiInitInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        ImGuiInitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        ImGuiInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_RenderImage.ImageFormat;
        ImGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&ImGuiInitInfo);

        ImGui_ImplVulkan_CreateFontsTexture();
    }

    void Renderer::Draw()
    {
        m_CommandLists[FrameIndex]->Wait();

        uint32_t ImageIndex = m_Swapchain->AcquireNextImage(FrameIndex);

        VkCommandBuffer Cmd = m_CommandLists[FrameIndex]->Begin();

        TransitionImageLayout(Cmd, m_RenderImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        TransitionImageLayout(Cmd, m_DepthImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            VK_IMAGE_ASPECT_DEPTH_BIT);

        auto Proj = glm::perspective(glm::radians(45.0f), 1280.f / (float)720.f, 0.1f, 1000.0f);
        //Vulkan's y-Axis is flipped compared to OpenGL based GLM
        Proj[1][1] *= -1;

        m_Camera.Move({ 0, 0, 0 });

        GraphicsDescriptor Desc{};
        Desc.View = m_Camera.GetViewMat();
        Desc.Proj = Proj;

        m_GPUMemoryAllocator->CopyDataToHost(m_GraphicsUBO, &Desc, sizeof(GraphicsDescriptor));

        VkClearColorValue ClearValue = { 1, 0, 1, 1 };
        VkRenderingAttachmentInfo TriangleColorAttachmentInfo = ColorAttachmentInfo(m_RenderImage.ImageView,
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &ClearValue);
        VkRenderingAttachmentInfo GraphicsDepthAttachmentInfo = DepthAttachmentInfo(m_DepthImage.ImageView,
                                                                                 VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, true);
        VkRenderingInfo TriangleRenderingInfo = RenderingInfo(1, &TriangleColorAttachmentInfo, &GraphicsDepthAttachmentInfo);

        vkCmdBeginRendering(Cmd, &TriangleRenderingInfo);

        vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetPipeline());

        VkViewport Viewport{};
        Viewport.x = 0.f;
        Viewport.y = 0.f;
        Viewport.width = (float)Application::Get().GetWindow().GetWidth();
        Viewport.height = (float)Application::Get().GetWindow().GetHeight();
        Viewport.minDepth = 0.f;
        Viewport.maxDepth = 1.f;

        vkCmdSetViewport(Cmd, 0, 1, &Viewport);

        VkRect2D Scissor{};
        Scissor.offset.x = 0;
        Scissor.offset.y = 0;
        Scissor.extent.width = Application::Get().GetWindow().GetWidth();
        Scissor.extent.height = Application::Get().GetWindow().GetHeight();

        vkCmdSetScissor(Cmd, 0, 1, &Scissor);

        struct PushConstants
        {
            glm::mat4 MVPMat;
            VkDeviceAddress DeviceAddr;
        } Consts{};

        Consts.DeviceAddr = m_MonkeyMesh.VertexBuffer.BufferAddress;

        auto Model = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));
        Consts.MVPMat = Model;

        vkCmdPushConstants(Cmd, m_GraphicsPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants),
            &Consts);

        vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->GetPipelineLayout(), 0, 1, &m_GraphicsDescriptor,
            0, nullptr);

        vkCmdBindIndexBuffer(Cmd, m_MonkeyMesh.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(Cmd, 2904, 1, 0, 0, 0);

        vkCmdEndRendering(Cmd);

        ImGui_ImplVulkan_NewFrame();
        ImGui::Render();

        VkRenderingAttachmentInfo ImGuiColorAttachmentInfo = ColorAttachmentInfo(m_RenderImage.ImageView,
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingInfo ImGuiRenderingInfo = RenderingInfo(1, &ImGuiColorAttachmentInfo);

        vkCmdBeginRendering(Cmd, &ImGuiRenderingInfo);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd);
        vkCmdEndRendering(Cmd);

        TransitionImageLayout(Cmd, m_RenderImage.Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        BlitImage(Cmd, m_RenderImage, { .Image = m_Swapchain->GetImage(ImageIndex), .ImageFormat = m_Swapchain->GetFormat().format, .ImageSize = m_Swapchain->GetExtent() });

        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        m_CommandLists[FrameIndex]->Submit(m_Swapchain->GetSemaphore(FrameIndex));
        m_Swapchain->Present(m_CommandLists[FrameIndex]);

        FrameIndex = (FrameIndex + 1) % FRAMES_IN_FLIGHT;
    }
}