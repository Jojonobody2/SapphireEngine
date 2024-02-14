#include "Descriptor.h"

#include "VkUtil.h"

namespace Sapphire
{
	void DescriptorLayoutBuilder::AddBinding(uint32_t Binding, VkDescriptorType Type)
	{
		VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding{};
		DescriptorSetLayoutBinding.binding = Binding;
		DescriptorSetLayoutBinding.descriptorCount = 1;
		DescriptorSetLayoutBinding.descriptorType = Type;

		DescriptorSetLayoutBindings.push_back(DescriptorSetLayoutBinding);
	}

	void DescriptorLayoutBuilder::Clear()
	{
		DescriptorSetLayoutBindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice Device, VkShaderStageFlags ShaderStage)
	{
		for (VkDescriptorSetLayoutBinding& Binding : DescriptorSetLayoutBindings)
		{
			Binding.stageFlags |= ShaderStage;
		}

		VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo{};
		DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		DescriptorSetLayoutCreateInfo.pNext = nullptr;
		DescriptorSetLayoutCreateInfo.flags = 0;
		DescriptorSetLayoutCreateInfo.bindingCount = (uint32_t)DescriptorSetLayoutBindings.size();
		DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.data();

		VkDescriptorSetLayout DescriptorSetLayout;
		VkCheckResult(vkCreateDescriptorSetLayout(Device, &DescriptorSetLayoutCreateInfo, nullptr, &DescriptorSetLayout));

		return DescriptorSetLayout;
	}

	void DescriptorAllocator::InitDescriptorPool(VkDevice Device, uint32_t MaxSets, std::span<PoolSizeRatio> PoolRatios)
	{
		std::vector<VkDescriptorPoolSize> PoolSizes;
		for (PoolSizeRatio Ratio : PoolRatios)
		{
			PoolSizes.push_back(VkDescriptorPoolSize
				{
					.type = Ratio.Type,
					.descriptorCount = uint32_t(Ratio.Ratio * MaxSets)
				});
		}

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo{};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.flags = 0;
		DescriptorPoolCreateInfo.maxSets = MaxSets;
		DescriptorPoolCreateInfo.poolSizeCount = (uint32_t)PoolSizes.size();
		DescriptorPoolCreateInfo.pPoolSizes = PoolSizes.data();

		VkCheckResult(vkCreateDescriptorPool(Device, &DescriptorPoolCreateInfo, nullptr, &DescriptorPool));
	}

	void DescriptorAllocator::ClearDescriptors(VkDevice Device)
	{
		VkCheckResult(vkResetDescriptorPool(Device, DescriptorPool, 0));
	}

	void DescriptorAllocator::DestroyDescriptorPool(VkDevice Device)
	{
		vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
	}

	VkDescriptorSet DescriptorAllocator::AllocateSet(VkDevice Device, VkDescriptorSetLayout DescriptorSetLayout) const
	{
		VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo{};
		DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		DescriptorSetAllocateInfo.pNext = nullptr;
		DescriptorSetAllocateInfo.descriptorPool = DescriptorPool;
		DescriptorSetAllocateInfo.descriptorSetCount = 1;
		DescriptorSetAllocateInfo.pSetLayouts = &DescriptorSetLayout;

		VkDescriptorSet DescriptorSet;
		VkCheckResult(vkAllocateDescriptorSets(Device, &DescriptorSetAllocateInfo, &DescriptorSet));

		return DescriptorSet;
	}
}