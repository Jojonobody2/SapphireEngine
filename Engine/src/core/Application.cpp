#include "Application.h"

#include <iostream>

namespace Sapphire
{
	Application::Application(const ApplicationCreateInfo& ApplicationCI)
	{
		std::cout << ApplicationCI.Name << std::endl;

		m_IsRunning = true;
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			Close();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}
}