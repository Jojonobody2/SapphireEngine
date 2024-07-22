#include "Application.h"

#include "Sapphire/Logging/Logger.h"
#include "Sapphire/ImGui/ImGuiLayer.h"
#include "Time.h"

namespace Sapphire
{
    Application* Application::s_Application = nullptr;

    Application::Application(const ApplicationInfo& AppInfo)
            : m_Name(AppInfo.Name)
    {
        s_Application = this;

        Logger::Init();
        SAPPHIRE_ENGINE_INFO("Initializing Engine");

        m_Window = IWindow::CreateWindow(m_Name, 1280, 720);
        m_Window->SetEventCallback(EventCallback);

        PushLayer(new ImGuiLayer());

        m_Renderer = CreateSharedPtr<Renderer>();

        PushLayer(AppInfo.BaseLayer);

        m_LastTime = Time::GetTimeMillis();
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
            double CurrentTime = Time::GetTimeMillis();
            m_DeltaTime = CurrentTime - m_LastTime;
            m_LastTime = CurrentTime;

            m_Window->PollEvents();

            for (ILayer* Layer : m_Layers)
            {
                Layer->OnUpdate(m_DeltaTime);
            }

            m_Renderer->Draw();
        }
    }

    void Application::PushLayer(ILayer* Layer)
    {
        Layer->OnAttach();
        m_Layers.push_back(Layer);
    }

    void Application::EventCallback(Event &Event)
    {
        for (ILayer* Layer : Application::Get().m_Layers)
        {
            Layer->OnEvent(Event);
        }
    }
}
