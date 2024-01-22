#include "GLFWWindow.h"

#include <iostream>

namespace Sapphire
{
	GlfwWindow::GlfwWindow(const WindowCreateInfo& WindowCI)
	{
		glfwInit();
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		m_Window = glfwCreateWindow(WindowCI.Width, WindowCI.Height, WindowCI.Title, nullptr, nullptr);
		
		const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (VidMode->width - WindowCI.Width) / 2, (VidMode->height - WindowCI.Height) / 2);
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
}