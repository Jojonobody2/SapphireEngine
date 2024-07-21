#pragma once

#include "RenderContext.h"

#include "Sapphire/Core/Base.h"

#include <vulkan/vulkan.h>

#include <string>
#include <filesystem>

namespace Sapphire
{
    class Shader
    {
    public:
        Shader(const std::filesystem::path& ShaderPath, const SharedPtr<RenderContext>& RenderContext);
        virtual ~Shader();

        VkShaderModule GetShaderModule() { return m_ShaderModule; }
        [[nodiscard]] VkShaderStageFlagBits GetShaderStage() const { return m_ShaderStage; }
        VkPipelineShaderStageCreateInfo GetShaderStageInfo() { return m_ShaderStageInfo; }

    private:
        VkShaderModule m_ShaderModule{};
        VkShaderStageFlagBits m_ShaderStage{};
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo{};

    private:
        SharedPtr<RenderContext> m_RenderContext{};
    };

    class GraphicsPipeline
    {
    public:

    private:

    };
}