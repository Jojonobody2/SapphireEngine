#pragma once

#include <glm/glm.hpp>

namespace Sapphire
{
	class Camera
	{
	public:
		explicit Camera(const glm::vec3& Pos = glm::vec3(0, 0, 0));

		void SetPos(const glm::vec3& Pos);
		void Move(const glm::vec3& DPos);
        void Rotate(float DX, float DZ);
		const glm::vec3& GetPos() { return m_Pos; }

		const glm::mat4& GetViewMat();

	private:
        glm::vec3 m_Pos{};
        float m_Pitch = 0.f;
        float m_Yaw = -90.f;

        const glm::vec3 m_UpDir = { 0.f, 1.f, 0.f };

		glm::mat4 m_ViewMat{};

	private:
		void CalculateView();
	};
}