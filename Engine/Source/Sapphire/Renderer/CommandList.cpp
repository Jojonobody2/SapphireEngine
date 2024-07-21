#include "CommandList.h"

#include "VulkanUtil.h"
#include "VulkanStructs.h"

namespace Sapphire
{
    CommandList::CommandList(const SharedPtr<RenderContext>& RenderContext)
    {
        m_RenderContext = RenderContext;

        VkCommandPoolCreateInfo CommandPoolCreateInfo = CommandPoolInfo(m_RenderContext->GetGraphicsQueue().Family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        VkCheck(vkCreateCommandPool(m_RenderContext->GetDevice(), &CommandPoolCreateInfo, nullptr, &m_CommandPool));

        VkCommandBufferAllocateInfo CmdAllocInfo = CommandBufferAllocInfo(m_CommandPool);
        VkCheck(vkAllocateCommandBuffers(m_RenderContext->GetDevice(), &CmdAllocInfo, &m_CommandBuffer));

        VkFenceCreateInfo FenceCreateInfo = FenceInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkCheck(vkCreateFence(m_RenderContext->GetDevice(), &FenceCreateInfo, nullptr, &m_Fence));

        VkSemaphoreCreateInfo SemaphoreCreateInfo = SemaphoreInfo();
        VkCheck(vkCreateSemaphore(m_RenderContext->GetDevice(), &SemaphoreCreateInfo, nullptr, &m_Semaphore));
    }

    CommandList::~CommandList()
    {
        vkDeviceWaitIdle(m_RenderContext->GetDevice());

        vkDestroySemaphore(m_RenderContext->GetDevice(), m_Semaphore, nullptr);
        vkDestroyFence(m_RenderContext->GetDevice(), m_Fence, nullptr);

        vkDestroyCommandPool(m_RenderContext->GetDevice(), m_CommandPool, nullptr);
    }

    void CommandList::Wait()
    {
        VkCheck(vkWaitForFences(m_RenderContext->GetDevice(), 1, &m_Fence, true, UINT64_MAX));
        VkCheck(vkResetFences(m_RenderContext->GetDevice(), 1, &m_Fence));
    }

    VkCommandBuffer CommandList::Begin()
    {
        VkCheck(vkResetCommandBuffer(m_CommandBuffer, 0));

        VkCommandBufferBeginInfo CmdBeginInfo = CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        VkCheck(vkBeginCommandBuffer(m_CommandBuffer, &CmdBeginInfo));

        return m_CommandBuffer;
    }

    void CommandList::Submit(VkSemaphore WaitSemaphore)
    {
        VkCheck(vkEndCommandBuffer(m_CommandBuffer));

        VkCommandBufferSubmitInfo CmdSubmitInfo = CommandBufferSubmitInfo(m_CommandBuffer);
        VkSemaphoreSubmitInfo WaitSemaphoreInfo = SemaphoreSubmitInfo(WaitSemaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
        VkSemaphoreSubmitInfo SignalSemaphoreInfo = SemaphoreSubmitInfo(m_Semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);

        VkSubmitInfo2 CLSubmitInfo = SubmitInfo(1, &WaitSemaphoreInfo, 1,
                                                &SignalSemaphoreInfo, 1, &CmdSubmitInfo);

        VkCheck(vkQueueSubmit2(m_RenderContext->GetGraphicsQueue().DeviceQueue, 1, &CLSubmitInfo, m_Fence));
    }
}