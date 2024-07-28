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
            m_CommandLists.push_back(CreateSharedPtr<CommandList>(m_RenderContext, true));

        m_GPUMemoryAllocator = CreateSharedPtr<GPUMemoryAllocator>(m_RenderContext);
        m_DescriptorAllocator = CreateSharedPtr<DescriptorAllocator>(m_RenderContext);

        m_RenderImage = m_GPUMemoryAllocator->AllocateImage(m_Swapchain->GetExtent(), VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));

        m_DepthImage = m_GPUMemoryAllocator->AllocateEmptyTexture(VK_FORMAT_D32_SFLOAT, m_Swapchain->GetExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                                  VK_IMAGE_ASPECT_DEPTH_BIT);

        CreatePipelines();
        InitImGui();

        BitmapImage DefaultImage = ImageImporter::Import("Resources/Models/sponza/white.png");
        m_DefaultTexture = m_GPUMemoryAllocator->UploadTexture(DefaultImage, VK_FORMAT_R8G8B8A8_UNORM);

        auto ModelData = ModelImporter::Import("Resources/Models/sponza/Sponza.gltf");
        for (auto& Mesh : ModelData.MeshDatas)
            m_Meshes.push_back({ m_GPUMemoryAllocator->UploadMesh(Mesh), Mesh.MaterialIndex });

        m_ModelTextures.resize(ModelData.MaterialDatas.size());

        for (const auto& Material : ModelData.MaterialDatas)
        {
            std::string MaterialPath = std::string("Resources/Models/sponza/") + Material.DiffuseTexPath;
            std::string NormalPath = std::string("Resources/Models/sponza/") + Material.NormalTexPath;
            std::string PBRPath = std::string("Resources/Models/sponza/") + Material.PBRTexPath;

            m_ModelTextures[Material.MaterialIndex].TextureSet = m_DescriptorAllocator->AllocateSet(m_GeometryMaterialSetLayout);

            DescriptorWriter Writer{};

            if (std::filesystem::exists(MaterialPath) && !std::filesystem::is_directory(MaterialPath))
            {
                BitmapImage DiffuseImage = ImageImporter::Import(MaterialPath);

                m_ModelTextures[Material.MaterialIndex].Texture = m_GPUMemoryAllocator->UploadTexture(DiffuseImage,
                                                                                                      VK_FORMAT_R8G8B8A8_UNORM);
                Writer.WriteTexture(0, m_ModelTextures[Material.MaterialIndex].Texture);
            }
            else
                Writer.WriteTexture(0, m_DefaultTexture);

            if (std::filesystem::exists(NormalPath) && !std::filesystem::is_directory(NormalPath))
            {
                BitmapImage NormalImage = ImageImporter::Import(NormalPath);

                m_ModelTextures[Material.MaterialIndex].NormalTex = m_GPUMemoryAllocator->UploadTexture(NormalImage,
                                                                                                        VK_FORMAT_R8G8B8A8_UNORM);

                Writer.WriteTexture(1, m_ModelTextures[Material.MaterialIndex].NormalTex);
            }
            else
                Writer.WriteTexture(1, m_DefaultTexture);

            if (std::filesystem::exists(PBRPath) && !std::filesystem::is_directory(PBRPath))
            {
                BitmapImage PBRImage = ImageImporter::Import(PBRPath);

                m_ModelTextures[Material.MaterialIndex].PBRTex = m_GPUMemoryAllocator->UploadTexture(PBRImage,
                                                                                                        VK_FORMAT_R8G8B8A8_UNORM);

                Writer.WriteTexture(2, m_ModelTextures[Material.MaterialIndex].PBRTex);
            }
            else
                Writer.WriteTexture(2, m_DefaultTexture);

            Writer.WriteSet(m_RenderContext, m_ModelTextures[Material.MaterialIndex].TextureSet);
        }
    }

    Renderer::~Renderer()
    {
        VkCheck(vkDeviceWaitIdle(m_RenderContext->GetDevice()));

        vkDestroyDescriptorSetLayout(m_RenderContext->GetDevice(), m_GeometryMaterialSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(m_RenderContext->GetDevice(), m_GeometryMatricesSetLayout, nullptr);

        for (auto& GeometryUBO : m_GeometryMatricesUBOs)
            m_GPUMemoryAllocator->DestroyBuffer(GeometryUBO);

        for (auto& Texture : m_ModelTextures)
        {
            if (Texture.Texture.Image.Image)
                m_GPUMemoryAllocator->DestroyTexture(Texture.Texture);
            if (Texture.NormalTex.Image.Image)
                m_GPUMemoryAllocator->DestroyTexture(Texture.NormalTex);
            if (Texture.PBRTex.Image.Image)
                m_GPUMemoryAllocator->DestroyTexture(Texture.PBRTex);
        }

        for (auto& Mesh : m_Meshes)
            m_GPUMemoryAllocator->DestroyMesh(Mesh.MeshBuffer);

        m_GPUMemoryAllocator->DestroyTexture(m_DefaultTexture);

        m_GPUMemoryAllocator->DestroyTexture(m_DepthImage);
        m_GPUMemoryAllocator->DestroyImage(m_RenderImage);

        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(m_RenderContext->GetDevice(), m_ImGuiPool, nullptr);

        SAPPHIRE_ENGINE_INFO("Shutting Rendering Engine down!");
    }

    void Renderer::Draw()
    {
        m_CommandLists[FrameIndex]->Wait();

        uint32_t ImageIndex = m_Swapchain->AcquireNextImage(FrameIndex);

        VkCommandBuffer Cmd = m_CommandLists[FrameIndex]->Begin();

        TransitionImageLayout(Cmd, m_RenderImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                              ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
        TransitionImageLayout(Cmd, m_DepthImage.Image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT));

        GraphicsDescriptor Desc{};
        Desc.View = m_Scene.GetCamera().GetViewMat();
        Desc.Proj = m_Scene.GetProjMat();

        m_GPUMemoryAllocator->CopyDataToHost(m_GeometryMatricesUBOs[FrameIndex], &Desc, sizeof(GraphicsDescriptor));

        VkClearColorValue ClearValue = { 1, 0, 1, 1 };
        VkRenderingAttachmentInfo TriangleColorAttachmentInfo = ColorAttachmentInfo(m_RenderImage.ImageView,
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &ClearValue);
        VkRenderingAttachmentInfo GraphicsDepthAttachmentInfo = DepthAttachmentInfo(m_DepthImage.Image.ImageView,
                                                                                 VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, true);

        VkRenderingInfo TriangleRenderingInfo = RenderingInfo(1, &TriangleColorAttachmentInfo, &GraphicsDepthAttachmentInfo);

        vkCmdBeginRendering(Cmd, &TriangleRenderingInfo);

        vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GeometryPipeline->GetPipeline());

        VkViewport GraphicsViewport = ViewportInfo((float)m_RenderImage.ImageSize.width, (float)m_RenderImage.ImageSize.height);
        vkCmdSetViewport(Cmd, 0, 1, &GraphicsViewport);

        VkRect2D GraphicsScissor = ScissorInfo((int32_t)m_RenderImage.ImageSize.width, (int32_t)m_RenderImage.ImageSize.height);
        vkCmdSetScissor(Cmd, 0, 1, &GraphicsScissor);

        for (auto& Mesh : m_Meshes)
        {
            GraphicsConstants PushConstants{};
            PushConstants.Model = m_Transform.GetMat();
            PushConstants.VertexBuffer = Mesh.MeshBuffer.VertexBuffer.BufferAddress;

            vkCmdPushConstants(Cmd, m_GeometryPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GraphicsConstants),
                &PushConstants);
           
            VkDescriptorSet Set[] =
            {
                m_GeometryMatricesSets[FrameIndex],
                m_ModelTextures[Mesh.MaterialIndex].TextureSet,
            };

            vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GeometryPipeline->GetPipelineLayout(), 0,
                                    2, Set,0, nullptr);

            vkCmdBindIndexBuffer(Cmd, Mesh.MeshBuffer.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(Cmd, Mesh.MeshBuffer.DrawCount, 1, 0, 0, 0);
        }

        vkCmdEndRendering(Cmd);

        ImGui_ImplVulkan_NewFrame();
        ImGui::Render();

        VkRenderingAttachmentInfo ImGuiColorAttachmentInfo = ColorAttachmentInfo(m_RenderImage.ImageView,
                                                                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingInfo ImGuiRenderingInfo = RenderingInfo(1, &ImGuiColorAttachmentInfo);

        vkCmdBeginRendering(Cmd, &ImGuiRenderingInfo);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Cmd);
        vkCmdEndRendering(Cmd);

        TransitionImageLayout(Cmd, m_RenderImage.Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));

        BlitImage(Cmd, m_RenderImage, { .Image = m_Swapchain->GetImage(ImageIndex), .ImageFormat = m_Swapchain->GetFormat().format, .ImageSize = m_Swapchain->GetExtent() });

        TransitionImageLayout(Cmd, m_Swapchain->GetImage(ImageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));

        m_CommandLists[FrameIndex]->Submit(m_Swapchain->GetSemaphore(FrameIndex));
        m_Swapchain->Present(m_CommandLists[FrameIndex]);

        FrameIndex = (FrameIndex + 1) % FRAMES_IN_FLIGHT;
    }

    void Renderer::CreatePipelines()
    {
        DescriptorSetLayoutBuilder DescriptorBuilder{};
        DescriptorWriter DescriptorWrite{};

        //geometry pass

        SharedPtr<Shader> GeometryVS = CreateSharedPtr<Shader>("Resources/Shaders/Geometry.vert.spv", m_RenderContext);
        SharedPtr<Shader> GeometryFS = CreateSharedPtr<Shader>("Resources/Shaders/Geometry.frag.spv", m_RenderContext);

        DescriptorBuilder.AddUBO(0, VK_SHADER_STAGE_VERTEX_BIT);
        m_GeometryMatricesSetLayout = DescriptorBuilder.Build(m_RenderContext);

        DescriptorBuilder.AddTexture(0);
        DescriptorBuilder.AddTexture(1);
        DescriptorBuilder.AddTexture(2);
        m_GeometryMaterialSetLayout = DescriptorBuilder.Build(m_RenderContext);

        VkDescriptorSetLayout GeometryDescriptors[] =
                {
                        m_GeometryMatricesSetLayout,
                        m_GeometryMaterialSetLayout
                };

        VkPushConstantRange GeometryConstants{};
        GeometryConstants.offset = 0;
        GeometryConstants.size = sizeof(GraphicsConstants);
        GeometryConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        GraphicsPipelineInfo GeometryPipelineInfo{};
        GeometryPipelineInfo.VertexShader = GeometryVS;
        GeometryPipelineInfo.FragmentShader = GeometryFS;
        GeometryPipelineInfo.ColorAttachmentCount = 1;
        GeometryPipelineInfo.pColorAttachments = &m_RenderImage.ImageFormat;
        GeometryPipelineInfo.pDepthAttachmentFormat = &m_DepthImage.Image.ImageFormat;
        GeometryPipelineInfo.Wireframe = false;
        GeometryPipelineInfo.DescriptorCount = 2;
        GeometryPipelineInfo.pDescriptors = GeometryDescriptors;
        GeometryPipelineInfo.PushConstantRangeCount = 1;
        GeometryPipelineInfo.pPushConstantRanges = &GeometryConstants;

        m_GeometryPipeline = CreateSharedPtr<GraphicsPipeline>(m_RenderContext, GeometryPipelineInfo);

        m_GeometryMatricesSets.resize(FRAMES_IN_FLIGHT);
        m_GeometryMatricesUBOs.resize(FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            m_GeometryMatricesSets[i] = m_DescriptorAllocator->AllocateSet(m_GeometryMatricesSetLayout);

            m_GeometryMatricesUBOs[i] = m_GPUMemoryAllocator->AllocateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                     VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(GraphicsDescriptor));

            DescriptorWrite.WriteUBO(0, m_GeometryMatricesUBOs[i]);
            DescriptorWrite.WriteSet(m_RenderContext, m_GeometryMatricesSets[i]);
        }
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
}