workspace "Sapphire"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
vulkansdk = os.getenv("VULKAN_SDK")

group "Dependencies"
	include "Engine/ThirdParty/GLFW"
	include "Engine/ThirdParty/spdlog"
	include "Engine/ThirdParty/imgui"
	include "Engine/ThirdParty/fastgltf"
	include "Engine/ThirdParty/simdjson"
group ""

group "Core"
	include "Engine"
	include "Editor"
group ""
