#include <Sapphire/Core/Application.h>
#include <Sapphire/Core/EntryPoint.h>
#include <Sapphire/Core/ILayer.h>

class EditorLayer : public Sapphire::ILayer
{
public:
    void OnAttach() override
    {
        Sapphire::WindowModeWindowed WM{};
        WM.Width = 1280;
        WM.Height = 720;

        Sapphire::Application::Get().GetWindow().SetWindowMode(WM);
    }

    void OnUpdate(double Delta) override
    {
        ImGui::ShowDemoWindow();
    }

    void OnEvent(Sapphire::Event& Event) override
    {

    }

    void OnDetach() override
    {

    }
};

Sapphire::ApplicationInfo SapphireInitialize()
{
    Sapphire::ApplicationInfo AppInfo{};
    AppInfo.Name = "Sapphire Editor";
    AppInfo.BaseLayer = new EditorLayer();

    return AppInfo;
}