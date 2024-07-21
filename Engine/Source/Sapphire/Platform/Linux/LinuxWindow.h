#pragma once

#ifdef SAPPHIRE_LINUX

#include "Sapphire/Core/IWindow.h"
#include "Sapphire/Core/InputCodes.h"

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
        void SetIcon(BitmapImage& Image) override;

        uint32_t GetWidth() override;
        uint32_t GetHeight() override;

        std::vector<MonitorMode> GetMonitorModes() override;

        void SetWindowMode(WindowMode& WindowMode) override;
        void SetEventCallback(SapphireEventCallback EventCallbackFn) override;

        void* GetNativeWindow() override;

    private:
        GLFWwindow* m_Window{};
        SapphireEventCallback m_EventCallbackFn{};

    private:
        static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods);
        static void CharCallback(GLFWwindow* Window, uint32_t KeyCode);
        static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods);
        static void MouseScrollCallback(GLFWwindow* Window, double xOffset, double yOffset);
        static void MouseMovedCallback(GLFWwindow* Window, double xPos, double yPos);

        static KeyCode TranslateKey(int KeyCode);
        static MouseCode TranslateButton(int MouseButton);
    };
}

#endif