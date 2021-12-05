solution "VKSol"
  location "build"
  configurations {"Debug", "Release"}
  platforms {"x32", "x64", "Native"}

  configuration "Debug"
    defines {"__DEBUG__"}
    flags { "Symbols", "NoPCH" }
    targetdir "bin/Debug"
    objdir "bin/Debug/obj"

  configuration "Release"
    defines {"__RELEASE__"}
    flags { "Optimize", "NoPCH" }
    targetdir "bin/Release"
    objdir "bin/Release/obj"


project "VKDev"
  location "build/projects"
  kind "ConsoleApp"
  language "C++"

  includedirs {
    "./include/",
    "./deps/vulkan/Include"
  }

  files {
    "./include/*.h",
    "./include/functionlist.inl",
    "./src/*.cpp",

    -- GLFW files --
    "./deps/glfw/src/internal.h",
    "./deps/glfw/src/context.c",
    "./deps/glfw/src/init.c",
    "./deps/glfw/src/input.c",
    "./deps/glfw/src/monitor.c",
    "./deps/glfw/src/vulkan.c",
    "./deps/glfw/src/window.c",
    "./deps/glfw/src/egl_context.h",
    "./deps/glfw/src/egl_context.c",
    "./deps/glfw/src/osmesa_context.h",
    "./deps/glfw/src/osmesa_context.c",
    "./deps/glfw/include/GLFW/glfw3.h",
  }

  configuration "vs*" 
    defines {
      "VK_USE_PLATFORM_WIN32_KHR",
      "_GLFW_WIN32",
      "_GLFW_WGL",
    }

    files {
      -- GLFW dependencies for windows --
      "./deps/glfw/src/win32_platform.h",
      "./deps/glfw/src/win32_joystick.h",
      "./deps/glfw/src/win32_init.c",
      "./deps/glfw/src/win32_joystick.c",
      "./deps/glfw/src/win32_monitor.c",
      "./deps/glfw/src/win32_time.c",
      "./deps/glfw/src/win32_thread.c",
      "./deps/glfw/src/win32_window.c",
      "./deps/glfw/src/wgl_context.c",
      "./deps/glfw/src/wgl_context.h",
    }

    configuration "vs2015"
		  windowstargetplatformversion "8.1"

		configuration "vs2017"
			windowstargetplatformversion "10.0.17134.0"


  configuration {"linux", "gmake"}
    defines {
      "VK_USE_PLATFORM_XLIB_KHR",
      "_GLFW_X11",
    }
  
    links {"dl", "X11", "pthread"}

    files {
    -- GLFW dependencies for linux --
      "./deps/glfw/src/posix_thread.c",
      "./deps/glfw/src/posix_thread.h",
      "./deps/glfw/src/posix_time.h",
      "./deps/glfw/src/posix_time.c",
      "./deps/glfw/src/xkb_unicode.c",
      "./deps/glfw/src/xkb_unicode.h",
      "./deps/glfw/src/x11_platform.h",
      "./deps/glfw/src/x11_init.c",
      "./deps/glfw/src/x11_monitor.c",
      "./deps/glfw/src/x11_window.c",
      "./deps/glfw/src/glx_context.c",
      "./deps/glfw/src/glx_context.h",
      "./deps/glfw/src/linux_joystick.c",
      "./deps/glfw/src/linux_joystick.h",
    }

