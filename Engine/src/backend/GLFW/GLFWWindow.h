#pragma once

#include "core/Window.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
	class GlfwWindow : public Window
	{
	public:
		GlfwWindow(const WindowCreateInfo& WindowCI);
		virtual ~GlfwWindow();
		
		void Update() override;
		
		void Close() override;
		bool IsOpen() override;
		
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		
	private:
		GLFWwindow* m_Window;
	};
}