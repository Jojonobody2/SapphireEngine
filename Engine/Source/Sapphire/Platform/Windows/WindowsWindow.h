#pragma once

#ifdef SAPPHIRE_WINDOWS

#include "Sapphire/Core/IWindow.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
    class WindowsWindow : public IWindow
    {
    public:
        WindowsWindow(const std::string& Title, uint32_t Width, uint32_t Height);
        ~WindowsWindow() override;

        void PollEvents() override;
        bool IsOpen() override;

        void SetTitle(const std::string& Title) override;
        void SetIcon(Image& Image) override;

        std::vector<MonitorMode> GetMonitorModes() override;

        void SetWindowMode(WindowMode& WindowMode) override;

    private:
        GLFWwindow* m_Window{};
    };
}

#endif