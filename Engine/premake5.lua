project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Source/**.h",
		"Source/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	links
	{
		"GLFW"
	}

	includedirs
	{
		"Source",
		"ThirdParty/GLFW/include"
	}

	filter "system:linux"
		pic "On"
		systemversion "latest"
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
