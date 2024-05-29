#include "Application.h"

#include "Sapphire/Logging/Logger.h"

#include <iostream>

namespace Sapphire
{
    Application* Application::s_Application = nullptr;

    Application::Application(const ApplicationInfo& AppInfo)
            : m_Name(AppInfo.Name)
    {
        s_Application = this;

        Logger::Init();

        m_Window = IWindow::CreateWindow(m_Name, 1280, 720);

        PushLayer(AppInfo.BaseLayer);

        SAPPHIRE_ENGINE_INFO("Initializing Engine");
    }

    Application::~Application()
    {
        for (ILayer* Layer : m_Layers)
        {
            Layer->OnDetach();
            delete Layer;
        }

        SAPPHIRE_ENGINE_INFO("Shutting down Engine");
    }

    void Application::Run()
    {
        while (m_Window->IsOpen())
        {
            m_Window->PollEvents();

            for (ILayer* Layer : m_Layers)
            {
                Layer->OnUpdate();
            }
        }
    }

    void Application::PushLayer(ILayer* Layer)
    {
        Layer->OnAttach();
        m_Layers.push_back(Layer);
    }
}
