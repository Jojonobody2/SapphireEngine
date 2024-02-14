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
		bool IsMinimized() override;
		
		void SetTitle(const char* Title) override;
		void SetIcon(Image& Icon, Image& IconSm) override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void* GetNativeWindow() override
		{
			return m_Window;
		}
		
	private:
		GLFWwindow* m_Window;

		static void WindowCloseCallback(GLFWwindow* Window);
	};
}