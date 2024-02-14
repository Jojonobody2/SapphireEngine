#include "Types.h"

#include "VkUtil.h"

namespace Sapphire
{
	void ImmediateContext::CreateImmediateContext(VkDevice Device, uint32_t QueueIndex, VkQueue Queue)
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo{};
		CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCreateInfo.pNext = nullptr;
		CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CommandPoolCreateInfo.queueFamilyIndex = QueueIndex;

		VkCheckResult(vkCreateCommandPool(Device, &CommandPoolCreateInfo, nullptr, &m_ImmediateCommandPool));

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
		CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocateInfo.pNext = nullptr;
		CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandBufferAllocateInfo.commandBufferCount = 1;
		CommandBufferAllocateInfo.commandPool = m_ImmediateCommandPool;

		VkCheckResult(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &m_ImmediateCommandBuffer));

		VkFenceCreateInfo FenceCreateInfo{};
		FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCreateInfo.pNext = nullptr;
		FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkCheckResult(vkCreateFence(Device, &FenceCreateInfo, nullptr, &m_ImmediateFence));

		m_ImmediateQueue = Queue;
	}

	void ImmediateContext::SubmitImmediate(VkDevice Device, std::function<void(VkCommandBuffer Cmd)>&& Function)
	{
		VkCheckResult(vkResetFences(Device, 1, &m_ImmediateFence));
		VkCheckResult(vkResetCommandBuffer(m_ImmediateCommandBuffer, 0));

		VkCommandBuffer Cmd = m_ImmediateCommandBuffer;

		VkCommandBufferBeginInfo CmdBeginInfo{};
		CmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CmdBeginInfo.pNext = nullptr;
		CmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CmdBeginInfo.pInheritanceInfo = nullptr;

		VkCheckResult(vkBeginCommandBuffer(Cmd, &CmdBeginInfo));

		Function(Cmd);

		VkCheckResult(vkEndCommandBuffer(Cmd));

		VkCommandBufferSubmitInfo CmdSubmitInfo = CreateCmdSubmitInfo(Cmd);
		VkSubmitInfo2 SubmitInfo = CreateSubmitInfo(&CmdSubmitInfo, nullptr, nullptr);

		VkCheckResult(vkQueueSubmit2(m_ImmediateQueue, 1, &SubmitInfo, m_ImmediateFence));
		VkCheckResult(vkWaitForFences(Device, 1, &m_ImmediateFence, true, UINT64_MAX));
	}

	void ImmediateContext::DestroyImmediateContext(VkDevice Device)
	{
		vkDestroyFence(Device, m_ImmediateFence, nullptr);
		vkDestroyCommandPool(Device, m_ImmediateCommandPool, nullptr);
	}
}
