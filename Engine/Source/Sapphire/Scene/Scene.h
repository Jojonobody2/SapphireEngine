#pragma once

#include "Camera.h"

namespace Sapphire
{
    class Scene
    {
    public:
        Scene();
        virtual ~Scene() = default;

        Camera& GetCamera() { return m_Camera; }
        const glm::mat4& GetProjMat() { return m_ProjMat; }

    private:
        Camera m_Camera{};
        glm::mat4 m_ProjMat{};
    };
}