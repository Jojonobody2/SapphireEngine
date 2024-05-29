#include "IWindow.h"

#ifdef SAPPHIRE_LINUX
    #include "Sapphire/Platform/Linux/LinuxWindow.h"
#elif SAPPHIRE_WINDOWS
    #include "Sapphire/Platform/Windows/WindowsWindow.h"
#else
    #error "PLATFORM NOT IMPLEMENTED (YET?)"
#endif

namespace Sapphire
{
    SharedPtr<IWindow> IWindow::CreateWindow(const std::string& Title, uint32_t Width, uint32_t Height)
    {
    #ifdef SAPPHIRE_LINUX
        return CreateSharedPtr<LinuxWindow>(Title, Width, Height);
    #elif SAPPHIRE_WINDOWS
        return CreateSharedPtr<WindowsWindow>(Title, Width, Height);
    #else
        #error "PLATFORM NOT IMPLEMENTED (YET?)"
    #endif
    }
}