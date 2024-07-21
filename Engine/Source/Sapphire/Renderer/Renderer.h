#pragma once

#include "RenderContext.h"
#include "Swapchain.h"
#include "CommandList.h"

#include "Sapphire/Core/Base.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include <vector>

namespace Sapphire
{
    constexpr auto FRAMES_IN_FLIGHT = 3;

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

        VkDescriptorPool m_ImGuiPool{};

        uint32_t FrameIndex{};
    };
}