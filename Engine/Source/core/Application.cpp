#include "Application.h"

#include <iostream>

namespace Sapphire
{
	Application* Application::s_Application = nullptr;

	Application::Application(const ApplicationCreateInfo& ApplicationCI)
	{
		s_Application = this;

		WindowCreateInfo WindowCI{};
		WindowCI.Title = ApplicationCI.Name;
		WindowCI.Width = 1280;
		WindowCI.Height = 720;
		
		m_Window = Window::Create(WindowCI);
		
		m_IsRunning = true;
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_Window->Update();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}
}