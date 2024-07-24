#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Sapphire
{
	Camera::Camera(const glm::vec3& Pos)
        : m_Pos(Pos)
	{}

	void Camera::SetPos(const glm::vec3& Pos)
	{
		m_Pos = Pos;
	}

	void Camera::Move(const glm::vec3& DPos)
	{
        glm::vec3 CamDir =
                {
                        glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch)),
                        glm::sin(glm::radians(m_Pitch)),
                        glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch))
                };
        glm::vec3 Front = glm::normalize(CamDir);

		m_Pos -= DPos.z * Front;
        m_Pos += DPos.x * glm::normalize(glm::cross(Front, m_UpDir));
        //m_Pos.z += DPos.z;
	}

	void Camera::CalculateView()
	{
        glm::vec3 CamDir =
                {
                        glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch)),
                        glm::sin(glm::radians(m_Pitch)),
                        glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch))
                };

        m_ViewMat = glm::lookAt(m_Pos, m_Pos + glm::normalize(CamDir), m_UpDir);
	}

    void Camera::Rotate(float DX, float DZ)
    {
        m_Pitch += DX;
        m_Yaw += DZ;
    }

    const glm::mat4& Camera::GetViewMat()
    {
        CalculateView();
        return m_ViewMat;
    }
}