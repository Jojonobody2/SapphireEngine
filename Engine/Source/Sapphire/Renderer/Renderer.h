#pragma once

#include "RenderContext.h"
#include "Swapchain.h"
#include "CommandList.h"
#include "Pipeline.h"
#include "Descriptor.h"
#include "GPUMemory.h"

#include "Sapphire/Core/Base.h"

#include "Sapphire/Scene/Camera.h"
#include "Sapphire/Scene/Scene.h"
#include "Sapphire/Scene/Transform.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include <vector>

namespace Sapphire
{
    constexpr auto FRAMES_IN_FLIGHT = 3;

    struct GraphicsDescriptor
    {
        glm::mat4 View;
        glm::mat4 Proj;
    };

    struct GraphicsConstants
    {
        glm::mat4 Model;
        VkDeviceAddress VertexBuffer;
    };

    struct TexturedMesh
    {
        GPUMeshBuffer MeshBuffer;
        uint32_t MaterialIndex{};
    };

    struct MeshTexture
    {
        GPUTexture Texture;
        GPUTexture NormalTex;
        GPUTexture PBRTex;
        VkDescriptorSet TextureSet;
    };

    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        Camera& GetCamera() { return m_Scene.GetCamera(); }

        void Draw();

    private:
        SharedPtr<RenderContext> m_RenderContext;
        SharedPtr<Swapchain> m_Swapchain;
        std::vector<SharedPtr<CommandList>> m_CommandLists;

        SharedPtr<GPUMemoryAllocator> m_GPUMemoryAllocator{};
        SharedPtr<DescriptorAllocator> m_DescriptorAllocator{};

        GPUImage m_RenderImage{};
        GPUTexture m_DepthImage{};

        GPUTexture m_DefaultTexture{};

        std::vector<TexturedMesh> m_Meshes{};
        std::vector<MeshTexture> m_ModelTextures{};

        Scene m_Scene{};

        Transform m_Transform{};

        //geometry pass stuff
        SharedPtr<GraphicsPipeline> m_GeometryPipeline{};
        VkDescriptorSetLayout m_GeometryMatricesSetLayout{};
        std::vector<VkDescriptorSet> m_GeometryMatricesSets{};
        std::vector<GPUBuffer> m_GeometryMatricesUBOs{};
        VkDescriptorSetLayout m_GeometryMaterialSetLayout{};

        VkDescriptorPool m_ImGuiPool{};

        uint32_t FrameIndex{};

    private:
        void CreatePipelines();
        void InitImGui();
    };
}