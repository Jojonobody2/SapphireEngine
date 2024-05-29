#pragma once

#include "Base.h"
#include "IWindow.h"
#include "ILayer.h"

#include <string>
#include <vector>

namespace Sapphire
{
    struct ApplicationInfo
    {
        std::string Name;
        ILayer* BaseLayer;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationInfo& AppInfo);
        virtual ~Application();

        void Run();

        void PushLayer(ILayer* Layer);

        const std::string& GetName() { return m_Name; }
        IWindow& GetWindow() { return *m_Window; }
        static Application& Get() { return *s_Application; }

    private:
        std::string m_Name{};

        std::vector<ILayer*> m_Layers;

        SharedPtr<IWindow> m_Window{};

    private:
        static Application* s_Application;
    };
}