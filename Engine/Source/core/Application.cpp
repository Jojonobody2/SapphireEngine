#include "Application.h"

#include "Logger.h"

#include "imgui/ImGuiLayer.h"
#include "imgui/ImGuiDebuggerLogSink.h"

#include "renderer/RenderEngine.h"

#include <iostream>

namespace Sapphire
{
	Application* Application::s_Application = nullptr;

	Application::Application(const ApplicationCreateInfo& ApplicationCI)
		: m_Name(ApplicationCI.Name)
	{
		s_Application = this;

		m_Debugger = std::make_shared<Debugger>();

		Logger::Init(m_Debugger.get());

		WindowCreateInfo WindowCI{};
		WindowCI.Title = ApplicationCI.Name;
		WindowCI.Width = 1920;
		WindowCI.Height = 1080;
		
		m_Window = Window::Create(WindowCI);
		m_Window->SetEventCallback(Application::EventCallback);

		PushLayer(std::make_shared<ImGuiLayer>());

		RenderEngine::Init();

		PushLayer(ApplicationCI.BaseLayer);

		m_IsRunning = true;
	}

	Application::~Application()
	{
		for (int i = 0; i < m_Layers.size(); i++)
		{
			m_Layers[i]->OnDetach();
		}

		RenderEngine::Shutdown();
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_Window->Update();

			for (std::shared_ptr<Layer> Layer : m_Layers)
			{
				Layer->OnUpdate();
			}

			m_Debugger->Draw();

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

	void Application::PushLayer(std::shared_ptr<Layer> Layer)
	{
		Layer->OnAttach();
		m_Layers.push_back(Layer);
	}

	void Application::EventCallback(Event& Event)
	{
		for (std::shared_ptr<Layer> Layer : Application::Get().m_Layers)
		{
			Layer->OnEvent(Event);
		}
	}
}