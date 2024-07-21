#include <iostream>
#include "Pipeline.h"

#include "VulkanUtil.h"

#include "Sapphire/Import/File/BinaryFileImporter.h"

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
}