#include <iostream>
#include "Pipeline.h"

#include "VulkanUtil.h"

#include "Sapphire/Import/File/BinaryFileImporter.h"

#include <glm/glm.hpp>

namespace Sapphire
{
    Shader::Shader(const std::filesystem::path& ShaderPath, const SharedPtr<RenderContext>& RenderContext)
    {
        m_RenderContext = RenderContext;

        BinaryFile ShaderFile = BinaryFileImporter::Import(ShaderPath);

        VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
        ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderModuleCreateInfo.pNext = nullptr;
        ShaderModuleCreateInfo.flags = 0;
        ShaderModuleCreateInfo.codeSize = ShaderFile.BlobSize;
        ShaderModuleCreateInfo.pCode = (uint32_t*)ShaderFile.BinaryBlob.data();

        VkCheck(vkCreateShaderModule(m_RenderContext->GetDevice(), &ShaderModuleCreateInfo, nullptr, &m_ShaderModule));

        std::string ShaderType = ShaderPath.stem().extension().string();
        std::transform(ShaderType.begin(), ShaderType.end(), ShaderType.begin(), tolower);

        if (ShaderType == ".vert")
            m_ShaderStage = VK_SHADER_STAGE_VERTEX_BIT;
        else if (ShaderType == ".frag")
            m_ShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
        else if (ShaderType == ".comp")
            m_ShaderStage = VK_SHADER_STAGE_COMPUTE_BIT;

        m_ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStageInfo.pNext = nullptr;
        m_ShaderStageInfo.stage = m_ShaderStage;
        m_ShaderStageInfo.module = m_ShaderModule;
        m_ShaderStageInfo.pName = "main";
    }

    Shader::~Shader()
    {
        vkDestroyShaderModule(m_RenderContext->GetDevice(), m_ShaderModule, nullptr);
    }

    GraphicsPipeline::GraphicsPipeline(const SharedPtr<RenderContext>& RenderContext, const GraphicsPipelineInfo& GraphicsPipelineInfo)
    {
        m_RenderContext = RenderContext;

        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo{};
        PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutCreateInfo.pNext = nullptr;
        PipelineLayoutCreateInfo.flags = 0;
        PipelineLayoutCreateInfo.setLayoutCount = GraphicsPipelineInfo.DescriptorCount;
        PipelineLayoutCreateInfo.pSetLayouts = GraphicsPipelineInfo.pDescriptors;
        PipelineLayoutCreateInfo.pushConstantRangeCount = GraphicsPipelineInfo.PushConstantRangeCount;
        PipelineLayoutCreateInfo.pPushConstantRanges = GraphicsPipelineInfo.pPushConstantRanges;

        VkCheck(vkCreatePipelineLayout(m_RenderContext->GetDevice(), &PipelineLayoutCreateInfo, nullptr, &m_GraphicsPipelineLayout));

        VkPipelineShaderStageCreateInfo ShaderStages[] =
        {
            GraphicsPipelineInfo.VertexShader->GetShaderStageInfo(),
            GraphicsPipelineInfo.FragmentShader->GetShaderStageInfo(),
        };

        VkPipelineVertexInputStateCreateInfo InputStateCreateInfo{};
        InputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo{};
        InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        InputAssemblyStateCreateInfo.pNext = nullptr;
        InputAssemblyStateCreateInfo.flags = 0;
        InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo ViewportStateCreateInfo{};
        ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ViewportStateCreateInfo.pNext = nullptr;
        ViewportStateCreateInfo.flags = 0;
        ViewportStateCreateInfo.viewportCount = 1;
        ViewportStateCreateInfo.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo{};
        RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        RasterizationStateCreateInfo.pNext = nullptr;
        RasterizationStateCreateInfo.flags = 0;
        RasterizationStateCreateInfo.polygonMode = GraphicsPipelineInfo.Wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
        RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        RasterizationStateCreateInfo.lineWidth = 1.f;

        VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo{};
        MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        MultisampleStateCreateInfo.pNext = nullptr;
        MultisampleStateCreateInfo.flags = 0;
        MultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        MultisampleStateCreateInfo.minSampleShading = 1.f;
        MultisampleStateCreateInfo.pSampleMask = nullptr;
        MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        MultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo{};
        DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencilStateCreateInfo.pNext = nullptr;
        DepthStencilStateCreateInfo.flags = 0;
        if (GraphicsPipelineInfo.pDepthAttachmentFormat)
        {
            DepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
            DepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
            DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        }
        else
        {
            DepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
            DepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
            DepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
        }
        DepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        DepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
        DepthStencilStateCreateInfo.front = {};
        DepthStencilStateCreateInfo.back = {};
        DepthStencilStateCreateInfo.minDepthBounds = 0.f;
        DepthStencilStateCreateInfo.maxDepthBounds = 1.f;

        VkPipelineColorBlendAttachmentState ColorBlendAttachmentState{};
        ColorBlendAttachmentState.blendEnable = VK_TRUE;
        ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;;
        ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;

        std::vector<VkPipelineColorBlendAttachmentState> ColorBlendStates{};

        for (uint32_t i = 0; i < GraphicsPipelineInfo.ColorAttachmentCount; i++)
            ColorBlendStates.push_back(ColorBlendAttachmentState);

        VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo{};
        ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ColorBlendStateCreateInfo.pNext = nullptr;
        ColorBlendStateCreateInfo.flags = 0;
        ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        ColorBlendStateCreateInfo.attachmentCount = GraphicsPipelineInfo.ColorAttachmentCount;
        ColorBlendStateCreateInfo.pAttachments = ColorBlendStates.data();

        VkDynamicState DynamicStates[] =
        {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo{};
        DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        DynamicStateCreateInfo.pNext = nullptr;
        DynamicStateCreateInfo.flags = 0;
        DynamicStateCreateInfo.dynamicStateCount = 2;
        DynamicStateCreateInfo.pDynamicStates = DynamicStates;

        VkPipelineRenderingCreateInfo RenderingCreateInfo{};
        RenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        RenderingCreateInfo.pNext = nullptr;
        RenderingCreateInfo.colorAttachmentCount = GraphicsPipelineInfo.ColorAttachmentCount;
        RenderingCreateInfo.pColorAttachmentFormats = GraphicsPipelineInfo.pColorAttachments;
        if (GraphicsPipelineInfo.pDepthAttachmentFormat)
            RenderingCreateInfo.depthAttachmentFormat = *GraphicsPipelineInfo.pDepthAttachmentFormat;
        else
            RenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;

        VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo{};
        GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        GraphicsPipelineCreateInfo.pNext = &RenderingCreateInfo;
        GraphicsPipelineCreateInfo.flags = 0;
        GraphicsPipelineCreateInfo.stageCount = 2;
        GraphicsPipelineCreateInfo.pStages = ShaderStages;
        GraphicsPipelineCreateInfo.pVertexInputState = &InputStateCreateInfo;
        GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
        GraphicsPipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
        GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationStateCreateInfo;
        GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleStateCreateInfo;
        GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilStateCreateInfo;
        GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendStateCreateInfo;
        GraphicsPipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
        GraphicsPipelineCreateInfo.layout = m_GraphicsPipelineLayout;

        VkCheck(vkCreateGraphicsPipelines(m_RenderContext->GetDevice(), VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, nullptr, &m_GraphicsPipeline));
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        vkDestroyPipeline(m_RenderContext->GetDevice(), m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_RenderContext->GetDevice(), m_GraphicsPipelineLayout, nullptr);
    }
}