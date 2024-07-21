#pragma once

#include <string>
#include <vector>
#include "Base.h"
#include "Image.h"
#include "Sapphire/Event/SapphireEvent.h"

namespace Sapphire
{
    struct MonitorMode
    {
        int32_t Width, Height;
        int32_t RefreshRate;
    };

    enum class WindowType
    {
        WINDOW_TYPE_NULL,
        WINDOW_TYPE_WINDOWED,
        WINDOW_TYPE_MAXIMIZED,
        WINDOW_TYPE_EXCLUSIVE_FULLSCREEN,
        WINDOW_TYPE_BORDERLESS_FULLSCREEN
    };

    struct WindowMode
    {
    public:
        virtual WindowType GetType() { return WindowType::WINDOW_TYPE_NULL; }
    };

    //Leaving X or Y empty will center the corresponding axis
    struct WindowModeWindowed : public WindowMode
    {
        int32_t X = -1, Y = -1;
        int32_t Width{}, Height{};

    protected:
        WindowType GetType() override { return WindowType::WINDOW_TYPE_WINDOWED; }
    };

    struct WindowModeMaximized : public WindowMode
    {
    protected:
        WindowType GetType() override { return WindowType::WINDOW_TYPE_MAXIMIZED; }
    };

    struct WindowModeExclusiveFullscreen : public WindowMode
    {
        MonitorMode FullscreenMode{};

    protected:
        WindowType GetType() override { return WindowType::WINDOW_TYPE_EXCLUSIVE_FULLSCREEN; }
    };

    struct WindowModeBorderlessFullscreen : public WindowMode
    {
    protected:
        WindowType GetType() override { return WindowType::WINDOW_TYPE_BORDERLESS_FULLSCREEN; }
    };

    typedef void (*SapphireEventCallback)(Event& Event);

    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void PollEvents() = 0;
        virtual bool IsOpen() = 0;

        virtual void SetTitle(const std::string& Name) = 0;
        virtual void SetIcon(BitmapImage& Image) = 0;

        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;

        virtual std::vector<MonitorMode> GetMonitorModes() = 0;

        virtual void SetWindowMode(WindowMode& WindowMode) = 0;
        virtual void SetEventCallback(SapphireEventCallback EventCallbackFn) = 0;

        virtual void* GetNativeWindow() = 0;

        static SharedPtr<IWindow> CreateWindow(const std::string& Title, uint32_t Width, uint32_t Height);
    };
}