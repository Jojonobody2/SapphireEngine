project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"ThirdParty/stb_image/**.h",
		"ThirdParty/stb_image/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	links
	{
		"GLFW",
		"spdlog",
		"imgui",
		vulkansdk .. "/Lib/vulkan-1.lib"
	}

	includedirs
	{
		"Source",
		"ThirdParty/GLFW/include",
		"ThirdParty/spdlog/include",
		"ThirdParty/stb_image",
		"ThirdParty/imgui",
		vulkansdk .. "/Include"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		defines "SAPPHIRE_LINUX"

	filter "system:windows"
		systemversion "latest"
		defines "SAPPHIRE_WINDOWS"
	
	filter "configurations:Debug"
		defines
		{
			"SAPPHIRE_DEBUG",
			"SAPPHIRE_RENDER_DEBUG"
		}
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SAPPHIRE_RELEASE"
		runtime "Debug"
		optimize "On"
