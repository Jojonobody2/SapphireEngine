#ifdef SAPPHIRE_WINDOWS

#include "Sapphire/Core/Time.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
    double Time::GetTimeMillis()
    {
        return glfwGetTime();
    }
}

#endif