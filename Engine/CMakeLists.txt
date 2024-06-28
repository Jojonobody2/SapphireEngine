cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_WAYLAND FALSE)

add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/spdlog)
add_subdirectory(ThirdParty/libwebp)

project(Engine)

if (WIN32)
    set(OS "Windows")
elseif (UNIX AND NOT APPLE)
    set(OS "Linux")
else()
    set(OS "Undefined")
endif()

find_package(Vulkan REQUIRED)

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
        Source/Sapphire/Import/Image/Impl/WebPImageImporter.cpp
        Source/Sapphire/Import/Image/Impl/WebPImageImporter.h
        Source/Sapphire/Import/File/BinaryFileImporter.cpp
        Source/Sapphire/Import/File/BinaryFileImporter.h
        Source/Sapphire/Event/SapphireEvent.h
        Source/Sapphire/Event/MouseEvent.h
        Source/Sapphire/Event/KeyEvent.h
        Source/Sapphire/Core/InputCodes.h
        Source/Sapphire/RHI/IRenderContext.cpp
        Source/Sapphire/RHI/IRenderContext.h
        Source/Sapphire/Platform/VulkanRHI/VulkanRenderContext.cpp
        Source/Sapphire/Platform/VulkanRHI/VulkanRenderContext.h
        Source/Sapphire/Renderer/Renderer.cpp
        Source/Sapphire/Renderer/Renderer.h
        Source/Sapphire/Platform/VulkanRHI/VulkanUtil.h
        Source/Sapphire/Platform/VulkanRHI/VulkanRenderExtensions.cpp
        Source/Sapphire/Platform/VulkanRHI/VulkanRenderExtensions.h
        Source/Sapphire/Platform/VulkanRHI/VulkanRenderPlatform.h
        Source/Sapphire/Platform/VulkanRHI/Linux/VulkanRenderPlatformLinux.cpp
        Source/Sapphire/Platform/VulkanRHI/Windows/VulkanRenderPlatformWindows.cpp
        Source/Sapphire/Platform/VulkanRHI/VulkanUtil.cpp
)

target_include_directories(Engine
        PRIVATE ThirdParty/glfw/include ThirdParty/stb ThirdParty/libwebp/src ${Vulkan_INCLUDE_DIRS}
        PUBLIC ThirdParty/spdlog/include Source)
target_link_libraries(Engine
        PRIVATE vulkan glfw webp
        PUBLIC spdlog)

if (OS STREQUAL "Windows")
    target_compile_definitions(Engine PUBLIC SAPPHIRE_WINDOWS)
elseif (OS STREQUAL "Linux")
    target_compile_definitions(Engine PUBLIC SAPPHIRE_LINUX)
else()
    target_compile_definitions(Engine PUBLIC SAPPHIRE_OS_UNDEFINED)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Engine PRIVATE SAPPHIRE_RENDER_DEBUG)
endif()