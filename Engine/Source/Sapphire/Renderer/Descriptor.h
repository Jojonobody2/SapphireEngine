#pragma once

#include "RenderContext.h"
#include "GPUResources.h"

#include "Sapphire/Core/Base.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire
{
	class DescriptorSetLayoutBuilder
	{
	public:
		DescriptorSetLayoutBuilder() { Clear(); }

		void AddUBO(uint32_t Binding, VkShaderStageFlags ShaderStages);

		void Clear();

		VkDescriptorSetLayout Build(const SharedPtr<RenderContext>& RenderContext);

	private:
		std::vector<VkDescriptorSetLayoutBinding> m_Bindings{};
	};

	class DescriptorAllocator
	{
	public:
		DescriptorAllocator(const SharedPtr<RenderContext>& RenderContext);
		virtual ~DescriptorAllocator();

		VkDescriptorSet AllocateSet(VkDescriptorSetLayout DescriptorLayout);

	private:
		VkDescriptorPool m_Pool{};

		SharedPtr<RenderContext> m_RenderContext;
	};

	class DescriptorWriter
	{
	public:
		DescriptorWriter() { Clear(); }

		void AddBuffer(uint32_t Binding, const GPUBuffer& Buffer, uint32_t Offset = 0);

		void WriteSet(const SharedPtr<RenderContext> RenderContext, VkDescriptorSet Set);

		void Clear();

	private:
		std::vector<VkDescriptorBufferInfo> m_BufferInfos{};
		std::vector<VkWriteDescriptorSet> m_Writes{};
	};
}