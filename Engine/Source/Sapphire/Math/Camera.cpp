#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Sapphire
{
	Camera::Camera(const glm::vec3& Pos)
	{
		CalculateView();
	}

	void Camera::SetPos(const glm::vec3& Pos)
	{
		m_Pos = Pos;
		CalculateView();
	}

	void Camera::Move(const glm::vec3& DPos)
	{
		m_Pos += DPos;
		CalculateView();
	}

	void Camera::CalculateView()
	{
		m_ViewMat = glm::mat4(1.f);
		m_ViewMat = glm::translate(m_ViewMat, -m_Pos);
	}
}