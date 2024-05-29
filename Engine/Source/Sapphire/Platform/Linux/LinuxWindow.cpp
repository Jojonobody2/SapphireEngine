#include <iostream>
#include "LinuxWindow.h"
#include "Sapphire/Logging/Logger.h"

namespace Sapphire
{
    LinuxWindow::LinuxWindow(const std::string& Title, uint32_t Width, uint32_t Height)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow((int32_t)Width, (int32_t)Height, Title.c_str(), nullptr, nullptr);

        const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(m_Window, (VidMode->width - Width) / 2, (VidMode->height - Height) / 2);
    }

    LinuxWindow::~LinuxWindow()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void LinuxWindow::PollEvents()
    {
        glfwPollEvents();
    }

    bool LinuxWindow::IsOpen()
    {
        return !glfwWindowShouldClose(m_Window);
    }

    void LinuxWindow::SetTitle(const std::string& Title)
    {
        glfwSetWindowTitle(m_Window, Title.c_str());
    }

    std::vector<MonitorMode> LinuxWindow::GetMonitorModes()
    {
        std::vector<MonitorMode> MonitorModes;

        int32_t VidModeCount{};
        const GLFWvidmode* VidModes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &VidModeCount);

        for (int32_t i = 0; i < VidModeCount; i++)
        {
            MonitorModes.push_back({ VidModes[i].width, VidModes[i].height, VidModes[i].refreshRate });
        }

        return MonitorModes;
    }

    void LinuxWindow::SetWindowMode(WindowMode& WindowMode)
    {
        switch(WindowMode.GetType())
        {
            case WindowType::WINDOW_TYPE_WINDOWED:
            {
                WindowModeWindowed WM = *reinterpret_cast<WindowModeWindowed*>(&WindowMode);

                int32_t VidModeCount{};
                const GLFWvidmode* VidModes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &VidModeCount);

                const GLFWvidmode VidMode = VidModes[VidModeCount - 1];

                int ActualX = WM.X;
                int ActualY = WM.Y;

                if (WM.X < 0)
                {
                    ActualX = (VidMode.width - WM.Width) / 2;
                }
                if (WM.Y < 0)
                {
                    ActualY = (VidMode.height - WM.Height) / 2;
                }

                glfwSetWindowMonitor(m_Window, nullptr, ActualX, ActualY, WM.Width, WM.Height, 0);
                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);

                break;
            }

            case WindowType::WINDOW_TYPE_MAXIMIZED:
            {
                //TODO: cache window size
                glfwSetWindowMonitor(m_Window, nullptr, 0, 0, 1920, 1080, 0);
                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);
                glfwMaximizeWindow(m_Window);

                break;
            }

            case WindowType::WINDOW_TYPE_EXCLUSIVE_FULLSCREEN:
            {
                WindowModeExclusiveFullscreen WM = *reinterpret_cast<WindowModeExclusiveFullscreen*>(&WindowMode);

                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, WM.FullscreenMode.Width, WM.FullscreenMode.Height, WM.FullscreenMode.RefreshRate);

                break;
            }

            case WindowType::WINDOW_TYPE_BORDERLESS_FULLSCREEN:
            {
                int32_t VidModeCount{};
                const GLFWvidmode* VidModes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &VidModeCount);

                const GLFWvidmode VidMode = VidModes[VidModeCount - 1];

                glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowMonitor(m_Window, nullptr, 0, 0, 1, 1, VidMode.refreshRate);
                glfwSetWindowSize(m_Window, VidMode.width, VidMode.height);

                break;
            }

            case WindowType::WINDOW_TYPE_NULL: default:
                break;
        }
    }

    void LinuxWindow::SetIcon(Image& Image)
    {
        GLFWimage GLFWImage{};
        GLFWImage.width = (int32_t)Image.Width;
        GLFWImage.height = (int32_t)Image.Height;
        GLFWImage.pixels = Image.Pixels.data();

        glfwSetWindowIcon(m_Window, 1, &GLFWImage);
    }
}