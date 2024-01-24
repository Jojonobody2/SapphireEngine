#include "Window.h"

#if defined(SAPPHIRE_WINDOWS) || defined(SAPPHIRE_LINUX)
	#include "backend/GLFW/GLFWWindow.h"
#else
	#error "OS not supported!"
#endif

#include "Application.h"

namespace Sapphire
{
	std::shared_ptr<Window> Window::Create(const WindowCreateInfo& WindowCI)
	{
	#if defined(SAPPHIRE_WINDOWS) || defined(SAPPHIRE_LINUX)
		return std::make_shared<GlfwWindow>(WindowCI);
	#else
		#error "OS not supported!"
	#endif
	}

	void Window::OnClose()
	{
		Application::Get().Close();
	}
}