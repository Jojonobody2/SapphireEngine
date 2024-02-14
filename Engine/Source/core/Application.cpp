#include "Application.h"

#include "Logger.h"

#include "renderer/RenderEngine.h"

#include <iostream>

namespace Sapphire
{
	Application* Application::s_Application = nullptr;

	Application::Application(const ApplicationCreateInfo& ApplicationCI)
		: m_Name(ApplicationCI.Name)
	{
		s_Application = this;

		Logger::Init();

		WindowCreateInfo WindowCI{};
		WindowCI.Title = ApplicationCI.Name;
		WindowCI.Width = 1280;
		WindowCI.Height = 720;
		
		m_Window = Window::Create(WindowCI);

		RenderEngine::Init();

		PushLayer(ApplicationCI.BaseLayer);

		m_IsRunning = true;
	}

	Application::~Application()
	{
		RenderEngine::Shutdown();
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_Window->Update();

			for (Layer* Layer : m_Layers)
			{
				Layer->OnUpdate();
			}

			if (m_Window->IsOpen() && !m_Window->IsMinimized())
			{
				RenderEngine::Submit();
			}
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}

	void Application::PushLayer(Layer* Layer)
	{
		Layer->OnAttach();
		m_Layers.push_back(Layer);
	}
}