#ifdef SAPPHIRE_LINUX

#include "Sapphire/Core/Time.h"

#include <GLFW/glfw3.h>

namespace Sapphire
{
    double Time::GetTimeMillis()
    {
        return glfwGetTime() * 1000;
    }
}

#endif