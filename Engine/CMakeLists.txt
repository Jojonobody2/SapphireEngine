cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_WAYLAND FALSE)

add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/spdlog)
add_subdirectory(ThirdParty/libwebp)
add_subdirectory(ThirdParty/imgui)
add_subdirectory(ThirdParty/assimp)

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
        Source/Sapphire/Renderer/RenderContext.cpp
        Source/Sapphire/Renderer/RenderContext.h
        Source/Sapphire/Renderer/Renderer.cpp
        Source/Sapphire/Renderer/Renderer.h
        Source/Sapphire/Renderer/VulkanUtil.h
        Source/Sapphire/Renderer/RenderExtensions.cpp
        Source/Sapphire/Renderer/RenderExtensions.h
        Source/Sapphire/Renderer/RenderPlatform.h
        Source/Sapphire/Renderer/Linux/VulkanRenderPlatformLinux.cpp
        Source/Sapphire/Renderer/Windows/VulkanRenderPlatformWindows.cpp
        Source/Sapphire/Renderer/VulkanUtil.cpp
        Source/Sapphire/Renderer/Swapchain.cpp
        Source/Sapphire/Renderer/Swapchain.h
        Source/Sapphire/Renderer/VulkanStructs.h
        Source/Sapphire/Renderer/VulkanStructs.cpp
        Source/Sapphire/Renderer/CommandList.cpp
        Source/Sapphire/Renderer/CommandList.h
        Source/Sapphire/ImGui/ImGuiLayer.cpp
        Source/Sapphire/ImGui/ImGuiLayer.h
        Source/Sapphire/Core/Time.h
        Source/Sapphire/Platform/Linux/LinuxTime.cpp
        Source/Sapphire/Platform/Windows/WindowsTime.cpp
        Source/Sapphire/Renderer/Pipeline.h
        Source/Sapphire/Renderer/Pipeline.cpp
        Source/Sapphire/Renderer/GPUResources.h
        Source/Sapphire/Renderer/GPUMemory.cpp
        Source/Sapphire/Renderer/GPUMemory.h
        Source/Sapphire/Import/Model/ModelImporter.cpp
        Source/Sapphire/Import/Model/ModelImporter.h
        Source/Sapphire/Import/Model/Impl/AssimpModelImporter.cpp
        Source/Sapphire/Import/Model/Impl/AssimpModelImporter.h
        Source/Sapphire/Renderer/Descriptor.h
        Source/Sapphire/Renderer/Descriptor.cpp
        Source/Sapphire/Scene/Camera.cpp
        Source/Sapphire/Scene/Camera.h
        Source/Sapphire/Scene/Scene.h
        Source/Sapphire/Scene/Scene.cpp
        Source/Sapphire/Scene/Transform.h
        Source/Sapphire/Scene/Transform.cpp
)

target_include_directories(Engine
        PRIVATE ThirdParty/glfw/include ThirdParty/stb ThirdParty/libwebp/src ThirdParty/VulkanMemoryAllocator ThirdParty/assimp/include ${Vulkan_INCLUDE_DIRS}
        PUBLIC ThirdParty/spdlog/include ThirdParty/imgui ThirdParty/glm Source)
target_link_libraries(Engine
        PRIVATE vulkan glfw webp assimp
        PUBLIC spdlog imgui)

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
