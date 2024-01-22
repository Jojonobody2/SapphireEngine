#include "Application.h"

#include <iostream>

namespace Sapphire
{
	Application::Application(const ApplicationCreateInfo& ApplicationCI)
	{
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
		while (m_Window->IsOpen())
		{
			m_Window->Update();
		}
		
		m_IsRunning = false;
	}

	void Application::Close()
	{
		m_Window->Close();
	}
}