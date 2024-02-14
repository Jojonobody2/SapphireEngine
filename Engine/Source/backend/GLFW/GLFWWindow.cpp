#include "GLFWWindow.h"

namespace Sapphire
{
	GlfwWindow::GlfwWindow(const WindowCreateInfo& WindowCI)
	{
		glfwInit();
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		m_Window = glfwCreateWindow(WindowCI.Width, WindowCI.Height, WindowCI.Title, nullptr, nullptr);
		
		glfwSetWindowUserPointer(m_Window, this);

		const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (VidMode->width - WindowCI.Width) / 2, (VidMode->height - WindowCI.Height) / 2);

		glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
	}
	
	GlfwWindow::~GlfwWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	
	void GlfwWindow::Update()
	{
		glfwPollEvents();
	}
	
	void GlfwWindow::Close()
	{
		glfwSetWindowShouldClose(m_Window, true);
	}
	
	bool GlfwWindow::IsOpen()
	{
		return !glfwWindowShouldClose(m_Window);
	}

	bool GlfwWindow::IsMinimized()
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	void GlfwWindow::SetTitle(const char* Title)
	{
		glfwSetWindowTitle(m_Window, Title);
	}

	void GlfwWindow::SetIcon(Image& Icon, Image& IconSm)
	{
		GLFWimage IconImages[2];
		IconImages[0].width = Icon.GetWidth();
		IconImages[0].height = Icon.GetHeight();
		IconImages[0].pixels = Icon.GetPixels();

		IconImages[1].width = IconSm.GetWidth();
		IconImages[1].height = IconSm.GetHeight();
		IconImages[1].pixels = IconSm.GetPixels();

		glfwSetWindowIcon(m_Window, 2, IconImages);
	}
	
	uint32_t GlfwWindow::GetWidth() const
	{
		int32_t pWidth;
		glfwGetWindowSize(m_Window, &pWidth, nullptr);
		
		return pWidth;
	}
	
	uint32_t GlfwWindow::GetHeight() const
	{
		int32_t pHeight;
		glfwGetWindowSize(m_Window, nullptr, &pHeight);
		
		return pHeight;
	}

	void GlfwWindow::WindowCloseCallback(GLFWwindow* Window)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);
		pWindow->OnClose();
	}
}