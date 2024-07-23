#pragma once

#include "RenderContext.h"
#include "Swapchain.h"
#include "CommandList.h"
#include "Pipeline.h"
#include "Descriptor.h"
#include "GPUMemory.h"

#include "Sapphire/Core/Base.h"

#include "Sapphire/Math/Camera.h"

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

    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        Camera& GetCamera() { return m_Camera; }

        void Draw();

    private:
        SharedPtr<RenderContext> m_RenderContext;
        SharedPtr<Swapchain> m_Swapchain;
        std::vector<SharedPtr<CommandList>> m_CommandLists;
        SharedPtr<GraphicsPipeline> m_GraphicsPipeline{};

        SharedPtr<GPUMemoryAllocator> m_GPUMemoryAllocator{};
        SharedPtr<DescriptorAllocator> m_DescriptorAllocator{};

        GPUImage m_RenderImage{};
        GPUImage m_DepthImage{};

        GPUMeshBuffer m_MonkeyMesh{};

        Camera m_Camera{};

        VkDescriptorSetLayout m_GraphicsSetLayout{};
        VkDescriptorSet m_GraphicsDescriptor{};
        GPUBuffer m_GraphicsUBO{};

        VkDescriptorPool m_ImGuiPool{};

        uint32_t FrameIndex{};

    private:
        void InitImGui();
    };
}