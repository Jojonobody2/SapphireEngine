cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/spdlog)

project(Engine)

if (WIN32)
    set(OS "Windows")
elseif (UNIX AND NOT APPLE)
    set(OS "Linux")
else()
    set(OS "Undefined")
endif()


add_library(Engine
        Source/Sapphire/Core/Application.cpp
        Source/Sapphire/Core/Application.h
        Source/Sapphire/Core/EntryPoint.h
        Source/Sapphire/Platform/Linux/LinuxWindow.h
        Source/Sapphire/Platform/Linux/LinuxWindow.cpp
        Source/Sapphire/Core/IWindow.cpp
        Source/Sapphire/Core/IWindow.h
        Source/Sapphire/Core/Base.h
        Source/Sapphire/Logging/Logger.h
        Source/Sapphire/Logging/Logger.cpp
        Source/Sapphire/Core/ILayer.h
        Source/Sapphire/Core/Image.h
        Source/Sapphire/Platform/Windows/WindowsWindow.cpp
        Source/Sapphire/Platform/Windows/WindowsWindow.h
        Source/Sapphire/Import/Image/ImageImporter.cpp
        Source/Sapphire/Import/Image/ImageImporter.h
        Source/Sapphire/Import/Image/Impl/STBImageImporter.cpp
        Source/Sapphire/Import/Image/Impl/STBImageImporter.h
)

target_include_directories(Engine
        PRIVATE ThirdParty/glfw/include ThirdParty/stb
        PUBLIC ThirdParty/spdlog/include Source)
target_link_libraries(Engine
        PRIVATE glfw
        PUBLIC spdlog)

if (OS STREQUAL "Windows")
    target_compile_definitions(Engine PUBLIC SAPPHIRE_WINDOWS)
elseif (OS STREQUAL "Linux")
    target_compile_definitions(Engine PUBLIC SAPPHIRE_LINUX)
else()
    target_compile_definitions(Engine PUBLIC SAPPHIRE_OS_UNDEFINED)
endif()