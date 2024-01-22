#include "Window.h"

#include "backend/GLFW/GLFWWindow.h"

#include "Application.h"

namespace Sapphire
{
	std::shared_ptr<Window> Window::Create(const WindowCreateInfo& WindowCI)
	{
		return std::make_shared<GlfwWindow>(WindowCI);
	}
}