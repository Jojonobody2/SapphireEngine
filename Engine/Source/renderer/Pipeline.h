#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire
{
	class GraphicsPipelineBuilder
	{
	public:
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
		VkPipelineRasterizationStateCreateInfo RasterizationState;
		VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
		VkPipelineMultisampleStateCreateInfo MultisampleState;
		VkPipelineLayout PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo DepthStencilState;
		VkPipelineRenderingCreateInfo RenderingInfo;
		VkFormat ColorAttachmentFormat;

		void Clear();

		void SetShaders(VkShaderModule VertexShader, VkShaderModule FragmentShader);
		void SetTopology(VkPrimitiveTopology Topology);
		void SetPolygonMode(VkPolygonMode PolygonMode);
		void SetCullMode(VkCullModeFlags CullMode, VkFrontFace FrontFace);
		void DisableMultisampling();
		void DisableBlending();
		void EnableBlendingAdditive();
		void EnableBlendingAlphaBlend();
		void SetColorAttachmentFormat(VkFormat Format);
		void SetDepthFormat(VkFormat Format);
		void DisableDepthTest();
		void EnableDepthTest(bool DepthWriteEnable, VkCompareOp Op);

		VkPipeline BuildPipeline(VkDevice Device);

	private:
		std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
	};

	class ComputePipelineBuilder
	{
	public:
		VkPipelineLayout PipelineLayout;

		void Clear();
		void SetShader(VkShaderModule Shader);

		VkPipeline BuildPipeline(VkDevice Device) const;

	private:
		VkPipelineShaderStageCreateInfo ShaderStage;
	};
}