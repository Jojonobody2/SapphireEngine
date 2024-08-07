#include "Descriptor.h"

#include "VulkanUtil.h"

namespace Sapphire
{
	void DescriptorSetLayoutBuilder::AddUBO(uint32_t Binding, VkShaderStageFlags ShaderStages)
	{
		m_Bindings.push_back(VkDescriptorSetLayoutBinding{});

		VkDescriptorSetLayoutBinding& BindingInfo = m_Bindings[m_Bindings.size() - 1];
		BindingInfo.binding = Binding;
		BindingInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		BindingInfo.descriptorCount = 1;
		BindingInfo.stageFlags = ShaderStages;
	}

	void DescriptorSetLayoutBuilder::AddTexture(uint32_t Binding)
	{
		m_Bindings.push_back(VkDescriptorSetLayoutBinding{});

		VkDescriptorSetLayoutBinding& BindingInfo = m_Bindings[m_Bindings.size() - 1];
		BindingInfo.binding = Binding;
		BindingInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		BindingInfo.descriptorCount = 1;
		BindingInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	void DescriptorSetLayoutBuilder::Clear()
	{
		m_Bindings.clear();
	}

	VkDescriptorSetLayout DescriptorSetLayoutBuilder::Build(const SharedPtr<RenderContext>& RenderContext)
	{
		VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo{};
		DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		DescriptorSetLayoutCreateInfo.pNext = nullptr;
		DescriptorSetLayoutCreateInfo.flags = 0;
		DescriptorSetLayoutCreateInfo.bindingCount = (uint32_t)m_Bindings.size();
		DescriptorSetLayoutCreateInfo.pBindings = m_Bindings.data();

		VkDescriptorSetLayout SetLayout{};
		VkCheck(vkCreateDescriptorSetLayout(RenderContext->GetDevice(), &DescriptorSetLayoutCreateInfo, nullptr, &SetLayout));

        Clear();

		return SetLayout;
	}

	DescriptorAllocator::DescriptorAllocator(const SharedPtr<RenderContext>& RenderContext)
	{
		m_RenderContext = RenderContext;

		VkDescriptorPoolSize PoolSizes[] =
		{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000,
		};

		VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo{};
		DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolCreateInfo.pNext = nullptr;
		DescriptorPoolCreateInfo.flags = 0;
		DescriptorPoolCreateInfo.maxSets = 1000;
		DescriptorPoolCreateInfo.poolSizeCount = std::size(PoolSizes);
		DescriptorPoolCreateInfo.pPoolSizes = PoolSizes;

		VkCheck(vkCreateDescriptorPool(m_RenderContext->GetDevice(), &DescriptorPoolCreateInfo, nullptr, &m_Pool));
	}
	
	DescriptorAllocator::~DescriptorAllocator()
	{
		vkDestroyDescriptorPool(m_RenderContext->GetDevice(), m_Pool, nullptr);
	}

	VkDescriptorSet DescriptorAllocator::AllocateSet(VkDescriptorSetLayout DescriptorLayout)
	{
		VkDescriptorSet Set{};
		
		VkDescriptorSetAllocateInfo DescriptorSetAllocInfo{};
		DescriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		DescriptorSetAllocInfo.pNext = nullptr;
		DescriptorSetAllocInfo.descriptorPool = m_Pool;
		DescriptorSetAllocInfo.descriptorSetCount = 1;
		DescriptorSetAllocInfo.pSetLayouts = &DescriptorLayout;

		VkCheck(vkAllocateDescriptorSets(m_RenderContext->GetDevice(), &DescriptorSetAllocInfo, &Set));

		return Set;
	}

	void DescriptorWriter::Clear()
	{
		m_BufferInfos.clear();
		m_Writes.clear();
	}

	void DescriptorWriter::WriteUBO(uint32_t Binding, const GPUBuffer& Buffer, uint32_t Offset)
	{
		VkDescriptorBufferInfo& BufferInfo = m_BufferInfos.emplace_back(VkDescriptorBufferInfo{});
		BufferInfo.buffer = Buffer.Buffer;
		BufferInfo.offset = Offset;
		BufferInfo.range = Buffer.BufferSize;

        VkWriteDescriptorSet SetWrite{};
		SetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		SetWrite.pNext = nullptr;
		SetWrite.dstBinding = Binding;
		SetWrite.descriptorCount = 1;
		SetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		SetWrite.pBufferInfo = &BufferInfo;

        m_Writes.push_back(SetWrite);
	}

	void DescriptorWriter::WriteTexture(uint32_t Binding, const GPUTexture& Texture)
	{
		VkDescriptorImageInfo& ImageInfo = m_ImageInfos.emplace_back(VkDescriptorImageInfo{});
		ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ImageInfo.imageView = Texture.Image.ImageView;
		ImageInfo.sampler = Texture.Sampler;

        VkWriteDescriptorSet SetWrite{};
		SetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		SetWrite.pNext = nullptr;
		SetWrite.dstBinding = Binding;
		SetWrite.descriptorCount = 1;
		SetWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		SetWrite.pImageInfo = &ImageInfo;

        m_Writes.push_back(SetWrite);
	}

	void DescriptorWriter::WriteSet(const SharedPtr<RenderContext>& RenderContext, VkDescriptorSet Set)
	{
		for (auto& Write : m_Writes)
			Write.dstSet = Set;

		vkUpdateDescriptorSets(RenderContext->GetDevice(), (uint32_t)m_Writes.size(), m_Writes.data(), 0, nullptr);

        Clear();
    }
}
