#pragma once

#include "Sapphire/Core/IWindow.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
    class LinuxWindow : public IWindow
    {
    public:
        LinuxWindow(const std::string& Title, uint32_t Width, uint32_t Height);
        ~LinuxWindow() override;

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