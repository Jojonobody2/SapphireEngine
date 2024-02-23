#include "Pipeline.h"

#include "VkUtil.h"

namespace Sapphire
{
	void GraphicsPipelineBuilder::Clear()
	{
		InputAssemblyState = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		RasterizationState = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		ColorBlendAttachmentState = {};
		MultisampleState = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		PipelineLayout = {};
		DepthStencilState = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		RenderingInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

		ShaderStages.clear();
	}

	void GraphicsPipelineBuilder::SetShaders(VkShaderModule VertexShader, VkShaderModule FragmentShader)
	{
		ShaderStages.clear();

		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo{};
		ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageCreateInfo.pNext = nullptr;
		ShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		ShaderStageCreateInfo.module = VertexShader;
		ShaderStageCreateInfo.pName = "main";

		ShaderStages.push_back(ShaderStageCreateInfo);

		ShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		ShaderStageCreateInfo.module = FragmentShader;

		ShaderStages.push_back(ShaderStageCreateInfo);
	}

	void GraphicsPipelineBuilder::SetTopology(VkPrimitiveTopology Topology)
	{
		InputAssemblyState.topology = Topology;
	}

	void GraphicsPipelineBuilder::SetPolygonMode(VkPolygonMode PolygonMode)
	{
		RasterizationState.polygonMode = PolygonMode;
		RasterizationState.lineWidth = 1.0f;
	}

	void GraphicsPipelineBuilder::SetCullMode(VkCullModeFlags CullMode, VkFrontFace FrontFace)
	{
		RasterizationState.cullMode = CullMode;
		RasterizationState.frontFace = FrontFace;
	}

	void GraphicsPipelineBuilder::DisableMultisampling()
	{
		MultisampleState.sampleShadingEnable = VK_FALSE;
		MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		MultisampleState.minSampleShading = 1.0f;
		MultisampleState.pSampleMask = nullptr;
		MultisampleState.alphaToCoverageEnable = VK_FALSE;
		MultisampleState.alphaToOneEnable = VK_FALSE;
	}

	void GraphicsPipelineBuilder::DisableBlending()
	{
		ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachmentState.blendEnable = VK_FALSE;
	}

	void GraphicsPipelineBuilder::EnableBlendingAdditive()
	{
		ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachmentState.blendEnable = VK_TRUE;
		ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	void GraphicsPipelineBuilder::EnableBlendingAlphaBlend()
	{
		ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachmentState.blendEnable = VK_TRUE;
		ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	void GraphicsPipelineBuilder::SetColorAttachmentFormat(VkFormat Format)
	{
		ColorAttachmentFormat = Format;

		RenderingInfo.colorAttachmentCount = 1;
		RenderingInfo.pColorAttachmentFormats = &ColorAttachmentFormat;
	}

	void GraphicsPipelineBuilder::SetDepthFormat(VkFormat Format)
	{
		RenderingInfo.depthAttachmentFormat = Format;
	}

	void GraphicsPipelineBuilder::DisableDepthTest()
	{
		DepthStencilState.depthTestEnable = VK_FALSE;
		DepthStencilState.depthWriteEnable = VK_FALSE;
		DepthStencilState.depthCompareOp = VK_COMPARE_OP_NEVER;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.stencilTestEnable = VK_FALSE;
		DepthStencilState.front = {};
		DepthStencilState.back = {};
		DepthStencilState.minDepthBounds = 0.0f;
		DepthStencilState.maxDepthBounds = 1.0f;
	}

	void GraphicsPipelineBuilder::EnableDepthTest(bool DepthWriteEnable, VkCompareOp Op)
	{
		DepthStencilState.depthTestEnable = VK_TRUE;
		DepthStencilState.depthWriteEnable = DepthWriteEnable;
		DepthStencilState.depthCompareOp = Op;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.stencilTestEnable = VK_FALSE;
		DepthStencilState.front = {};
		DepthStencilState.back = {};
		DepthStencilState.minDepthBounds = 0.0f;
		DepthStencilState.maxDepthBounds = 1.0f;
	}

	VkPipeline GraphicsPipelineBuilder::BuildPipeline(VkDevice Device)
	{
		VkPipelineViewportStateCreateInfo ViewportStateCreateInfo{};
		ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportStateCreateInfo.pNext = nullptr;
		ViewportStateCreateInfo.viewportCount = 1;
		ViewportStateCreateInfo.scissorCount = 1;

		VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo{};
		PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		PipelineColorBlendStateCreateInfo.pNext = nullptr;
		PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		PipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		PipelineColorBlendStateCreateInfo.pAttachments = &ColorBlendAttachmentState;
		PipelineColorBlendStateCreateInfo.attachmentCount = 1;

		VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo{};
		PipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo{};
		GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		GraphicsPipelineCreateInfo.pNext = &RenderingInfo;
		GraphicsPipelineCreateInfo.stageCount = (uint32_t)ShaderStages.size();
		GraphicsPipelineCreateInfo.pStages = ShaderStages.data();
		GraphicsPipelineCreateInfo.pVertexInputState = &PipelineVertexInputStateCreateInfo;
		GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
		GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
		GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationState;
		GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleState;
		GraphicsPipelineCreateInfo.pColorBlendState = &PipelineColorBlendStateCreateInfo;
		GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilState;
		GraphicsPipelineCreateInfo.layout = PipelineLayout;

		VkDynamicState DynamicState[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo{};
		PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		PipelineDynamicStateCreateInfo.dynamicStateCount = 2;
		PipelineDynamicStateCreateInfo.pDynamicStates = &DynamicState[0];

		GraphicsPipelineCreateInfo.pDynamicState = &PipelineDynamicStateCreateInfo;

		VkPipeline Pipeline;
		VkCheckResult(vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &Pipeline));

		return Pipeline;
	}

	void ComputePipelineBuilder::Clear()
	{
		PipelineLayout = {};
		ShaderStage = {};
	}

	void ComputePipelineBuilder::SetShader(VkShaderModule Shader)
	{
		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo{};
		ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageCreateInfo.pNext = nullptr;
		ShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		ShaderStageCreateInfo.module = Shader;
		ShaderStageCreateInfo.pName = "main";

		ShaderStage = ShaderStageCreateInfo;
	}

	VkPipeline ComputePipelineBuilder::BuildPipeline(VkDevice Device) const
	{
		VkComputePipelineCreateInfo ComputePipelineCreateInfo{};
		ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		ComputePipelineCreateInfo.pNext = nullptr;
		ComputePipelineCreateInfo.layout = PipelineLayout;
		ComputePipelineCreateInfo.stage = ShaderStage;

		VkPipeline Pipeline;
		VkCheckResult(vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1, &ComputePipelineCreateInfo, nullptr, &Pipeline));

		return Pipeline;
	}
}