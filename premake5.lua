workspace "nyeregfelulet"
	architecture "x86_64"
	startproject "nyeregfelulet"
	
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
VULKAN_SDK = os.getenv("VULKAN_SDK")

group "Dependencies"
externalproject "GLFW"
	location "nyeregfelulet/vendor/GLFW/src"
	uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

externalproject "glm_static"
	location "nyeregfelulet/vendor/glm/glm"
	uuid "57940020-8E99-AEB6-271F-61E0E7F6B73B"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
include "nyeregfelulet/vendor/ImGui"
include "nyeregfelulet/vendor/Glad"
group ""
	
project "nyeregfelulet"
	location "nyeregfelulet"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/stb_image",
		"%{VULKAN_SDK}/Include"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui"
	}
	
	defines
	{
		"NOMINMAX",
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_WINDOW"
	}

	postbuildcommands
	{
		("rmdir /s /q %{wks.location}x64")
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
		
		links
		{
			"%{VULKAN_SDK}/Lib/shaderc_sharedd.lib",
			"%{VULKAN_SDK}/Lib/spirv-cross-cored.lib",
			"%{VULKAN_SDK}/Lib/spirv-cross-glsld.lib"
		}
	
	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"
		
		links
		{
			"%{VULKAN_SDK}/Lib/shaderc_shared.lib",
			"%{VULKAN_SDK}/Lib/spirv-cross-core.lib",
			"%{VULKAN_SDK}/Lib/spirv-cross-glsl.lib"
		}