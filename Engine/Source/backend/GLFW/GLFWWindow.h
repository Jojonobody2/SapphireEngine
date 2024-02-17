#pragma once

#include "core/Window.h"
#include "core/KeyCodes.h"

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
		void SetIcon(Image Icon, Image IconSm) override;

		const char* GetClipboardString() override;
		void SetClipboardString(const char* String) override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void SetEventCallback(SapphireEventCallback EventCallbackFn)
		{
			m_EventCallbackFn = EventCallbackFn;
		}

		void* GetNativeWindow() override
		{
			return m_Window;
		}
		
	private:
		GLFWwindow* m_Window;
		SapphireEventCallback m_EventCallbackFn{};

		static KeyCode TranslateKey(int GlfwKeyCode);
		static MouseCode TranslateButton(int GlfwMouseButton);

		static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods);
		static void CharCallback(GLFWwindow* Window, uint32_t KeyCode);
		static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods);
		static void MouseScrollCallback(GLFWwindow* Window, double xOffset, double yOffset);
		static void MouseMovedCallback(GLFWwindow* Window, double xPos, double yPos);
		static void WindowCloseCallback(GLFWwindow* Window);
	};
}