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

group "Dependencies"
	include "Engine/ThirdParty/GLFW"
	include "Engine/ThirdParty/spdlog"
group ""

group "Core"
	include "Engine"
	include "Editor"
group ""
