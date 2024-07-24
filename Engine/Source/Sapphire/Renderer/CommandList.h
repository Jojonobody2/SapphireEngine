#pragma once

#include "RenderContext.h"
#include "Swapchain.h"
#include "Sapphire/Core/Base.h"

namespace Sapphire
{
    class CommandList
    {
    public:
        explicit CommandList(const SharedPtr<RenderContext>& RenderContext, bool SignaledFence = false);
        virtual ~CommandList();

        [[nodiscard]] VkSemaphore GetSemaphore() const { return m_Semaphore; }

        void Wait();
        VkCommandBuffer Begin();
        void Submit(VkSemaphore WaitSemaphore = nullptr);

    private:
        VkCommandPool m_CommandPool{};
        VkCommandBuffer m_CommandBuffer{};

        VkFence m_Fence{};
        VkSemaphore m_Semaphore{};

    private:
        SharedPtr<RenderContext> m_RenderContext;
    };
}
