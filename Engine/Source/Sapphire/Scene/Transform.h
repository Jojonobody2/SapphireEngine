#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Sapphire
{
    class Transform
    {
    public:
        Transform() = default;
        Transform(glm::vec3 Translation, glm::vec3 Scale, glm::quat Rotation);

        void SetTranslation(glm::vec3 Translation) { m_Translation = Translation; }
        void Translate(glm::vec3 TDelta) { m_Translation += TDelta; }
        glm::vec3 GetTranslation() { return m_Translation; }

        void SetScale(glm::vec3 Scale) { m_Scale = Scale; }
        void Scale(glm::vec3 SDelta) { m_Scale += SDelta; }
        glm::vec3 GetScale() { return m_Scale; }

        void SetRotation(glm::quat Rotation) { m_Rotation = Rotation; }
        void SetRotation(glm::vec3 Rotation) { m_Rotation = glm::quat(glm::radians(Rotation)); }
        void Rotate(glm::quat RDelta) { m_Rotation = m_Rotation * RDelta; }
        void Rotate(glm::vec3 RDelta) { m_Rotation = glm::quat(glm::radians(RDelta)) * m_Rotation; };
        glm::quat GetRotation() { return m_Rotation; }
        glm::vec3 GetRotationEuler() { return glm::degrees(glm::eulerAngles(m_Rotation)); }

        glm::mat4 GetMat();

    private:
        glm::vec3 m_Translation{};
        glm::vec3 m_Scale{ 1, 1, 1 };
        glm::quat m_Rotation{ 1, 0, 0, 0 };
    };
}