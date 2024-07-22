#pragma once

#include "RenderContext.h"
#include "Swapchain.h"
#include "CommandList.h"
#include "Pipeline.h"
#include "GPUMemory.h"

#include "Sapphire/Core/Base.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include <vector>

namespace Sapphire
{
    constexpr auto FRAMES_IN_FLIGHT = 3;

    struct Vertex
    {
        float x, y, z, padding;
    };

    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        void InitImGui();

        void Draw();

    private:
        SharedPtr<RenderContext> m_RenderContext;
        SharedPtr<Swapchain> m_Swapchain;
        std::vector<SharedPtr<CommandList>> m_CommandLists;
        SharedPtr<GraphicsPipeline> m_GraphicsPipeline{};

        SharedPtr<GPUMemoryAllocator> m_GPUMemoryAllocator{};

        GPUImage m_RenderImage{};

        GPUBufferAddressable m_VertexBuffer{};
        GPUBuffer m_IndexBuffer{};

        VkDescriptorPool m_ImGuiPool{};

        uint32_t FrameIndex{};
    };
}