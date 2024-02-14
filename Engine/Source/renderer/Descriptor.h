#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <span>

namespace Sapphire
{
	struct DescriptorLayoutBuilder
	{
		std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;

		void AddBinding(uint32_t Binding, VkDescriptorType Type);
		void Clear();

		VkDescriptorSetLayout Build(VkDevice Device, VkShaderStageFlags ShaderStage);
	};

	struct DescriptorAllocator
	{
		struct PoolSizeRatio
		{
			VkDescriptorType Type;
			float Ratio;
		};

		VkDescriptorPool DescriptorPool;

		void InitDescriptorPool(VkDevice Device, uint32_t MaxSets, std::span<PoolSizeRatio> PoolRatios);
		void ClearDescriptors(VkDevice Device);
		void DestroyDescriptorPool(VkDevice Device);

		VkDescriptorSet AllocateSet(VkDevice Device, VkDescriptorSetLayout DescriptorSetLayout) const;
	};
}