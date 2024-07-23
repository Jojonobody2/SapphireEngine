#pragma once

#include <glm/glm.hpp>

namespace Sapphire
{
	class Camera
	{
	public:
		Camera(const glm::vec3& Pos = glm::vec3(0, 0, 0));

		void SetPos(const glm::vec3& Pos);
		void Move(const glm::vec3& DPos);
		const glm::vec3& GetPos() { return m_Pos; }

		const glm::mat4& GetViewMat() { return m_ViewMat; }

	private:
		glm::vec3 m_Pos{};

		glm::mat4 m_ViewMat{};

	private:
		void CalculateView();
	};
}