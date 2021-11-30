solution "VKSol"
  location "build"
  configurations {"Debug", "Release"}
  platforms {"x32", "x64", "Native"}
  language "C++"

  configuration "Debug"
    defines {"__DEBUG__"}
    flags { "Symbols", "NoPCH" }
    targetdir "bin/Debug"
    objdir "bin/Debug"

  configuration "Release"
    defines {"__RELEASE__"}
    flags { "Optimize", "NoPCH" }
    targetdir "bin/Release"
    objdir "bin/Release"


project "VKDev"
  location "build/projects"
  kind "ConsoleApp"

  includedirs {
    "./include/",
    "./deps/vulkan/Include"
  }

  configuration "vs*" 
    defines {
      "VK_USE_PLATFORM_WIN32_KHR",
      "_GLFW_WIN32",
      "_GLFW_WGL",
    }

    files {
      "./include/*.h",
      "./include/functionlist.inl",
      "./src/*.cpp",



      -- GLFW files --
			"./deps/glfw/src/internal.h",
			"./deps/glfw/src/mappings.h",
			"./deps/glfw/src/context.c",
			"./deps/glfw/src/init.c",
			"./deps/glfw/src/input.c",
			"./deps/glfw/src/monitor.c",
			"./deps/glfw/src/vulkan.c",
			"./deps/glfw/src/window.c",
			"./deps/glfw/src/win32_platform.h",
			"./deps/glfw/src/win32_joystick.h",
			"./deps/glfw/src/wgl_context.h",
			"./deps/glfw/src/egl_context.h",
			"./deps/glfw/src/osmesa_context.h",
			"./deps/glfw/src/win32_init.c",
			"./deps/glfw/src/win32_joystick.c",
			"./deps/glfw/src/win32_monitor.c",
			"./deps/glfw/src/win32_time.c",
			"./deps/glfw/src/win32_thread.c",
			"./deps/glfw/src/win32_window.c",
			"./deps/glfw/src/wgl_context.c",
			"./deps/glfw/src/egl_context.c",
			"./deps/glfw/src/osmesa_context.c",
			"./deps/glfw/include/**.h",
    }


    configuration "vs2015"
		  windowstargetplatformversion "8.1"

		configuration "vs2017"
			windowstargetplatformversion "10.0.17134.0"