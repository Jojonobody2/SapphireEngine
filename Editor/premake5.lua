project "Editor"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Engine/Source"
	}

	links
	{
		"Engine"
	}
	
	filter "system:linux"
		pic "On"
		systemversion "latest"
		links
		{
			"GLFW",
			"spdlog"
		}
		defines "SAPPHIRE_LINUX"

	filter "system:windows"
		systemversion "latest"
		defines "SAPPHIRE_WINDOWS"
	
	filter "configurations:Debug"
		defines "SAPPHIRE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SAPPHIRE_RELEASE"
		runtime "Debug"
		optimize "On"
