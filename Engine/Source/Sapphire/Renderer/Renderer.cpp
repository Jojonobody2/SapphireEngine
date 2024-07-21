#include "Renderer.h"

#include "Pipeline.h"
#include "VulkanUtil.h"
#include "VulkanStructs.h"

#include "Sapphire/Logging/Logger.h"

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

        InitImGui();

        SharedPtr<Shader> VertexShader = CreateSharedPtr<Shader>("Resources/Shaders/Shader.vert.spv", m_RenderContext);
        SharedPtr<Shader> FragmentShader = CreateSharedPtr<Shader>("Resources/Shaders/Shader.frag.spv", m_RenderContext);

        m_GraphicsPipeline = CreateSharedPtr<GraphicsPipeline>(m_RenderContext, VertexShader, FragmentShader, m_Swapchain->GetFormat().format);
    }

    Renderer::~Renderer()
    {
        VkCheck(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

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

        VkFormat SurfaceFormat = m_Swapchain->GetFormat().format;

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
        ImGuiInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &SurfaceFormat;
        ImGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&ImGuiInitInfo);

        ImGui_ImplVulkan_CreateFontsTexture();
    }

    void Renderer::Draw()
    {
        m_CommandLists[FrameIndex]->Wait();

        uint32_t ImageIndex = m_Swapchain->AcquireNextImage(FrameIndex);

        VkCommandBuffer Cmd = m_CommandLists[FrameIndex]->Begin();

        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkClearColorValue ClearValue = { 1, 0, 1, 1 };
        VkRenderingAttachmentInfo TriangleColorAttachmentInfo = ColorAttachmentInfo(m_Swapchain->GetImageView(ImageIndex),
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &ClearValue);
        VkRenderingInfo TriangleRenderingInfo = RenderingInfo(1, &TriangleColorAttachmentInfo);

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

        vkCmdDraw(Cmd, 3, 1, 0, 0);

        vkCmdEndRendering(Cmd);

        ImGui_ImplVulkan_NewFrame();
        ImGui::Render();

        VkRenderingAttachmentInfo ImGuiColorAttachmentInfo = ColorAttachmentInfo(m_Swapchain->GetImageView(ImageIndex),
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingInfo ImGuiRenderingInfo = RenderingInfo(1, &ImGuiColorAttachmentInfo);

        vkCmdBeginRendering(Cmd, &ImGuiRenderingInfo);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd);
        vkCmdEndRendering(Cmd);

        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        m_CommandLists[FrameIndex]->Submit(m_Swapchain->GetSemaphore(FrameIndex));
        m_Swapchain->Present(m_CommandLists[FrameIndex]);

        FrameIndex = (FrameIndex + 1) % FRAMES_IN_FLIGHT;
    }
}