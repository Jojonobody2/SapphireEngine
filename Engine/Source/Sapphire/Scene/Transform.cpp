#include "Transform.h"

namespace Sapphire
{
    Transform::Transform(glm::vec3 Translation, glm::vec3 Scale, glm::quat Rotation)
        : m_Translation(Translation), m_Scale(Scale), m_Rotation(Rotation)
    {}

    glm::mat4 Transform::GetMat()
    {
        return glm::translate(glm::mat4(1.f), m_Translation) * glm::toMat4(m_Rotation) * glm::scale(glm::mat4(1.f), m_Scale);
    }
}

