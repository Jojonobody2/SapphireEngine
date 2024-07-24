#include "Scene.h"

#include "Sapphire/Core/Application.h"

#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

namespace Sapphire
{
    Scene::Scene()
        : m_Camera(glm::vec3(0, 2, 0))
    {
        m_ProjMat = glm::perspective(glm::radians(45.f),
                         (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(),
                         0.1f, 1000.0f);
        //Vulkan's y-Axis is flipped compared to OpenGL based GLM
        m_ProjMat[1][1] *= -1;
    }
}