#CMake config for GLFW files

set(LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/src")
IF (WIN32)
add_compile_definitions(_GLFW_WIN32 _GLFW_WGL)
add_library(GLFW "src/win32_init.c" 
                 "src/win32_joystick.c"
                 "src/win32_monitor.c"
                 "src/win32_time.c"
                 "src/win32_thread.c"
                 "src/win32_window.c"
                 "src/wgl_context.c")
ELSEIF (UNIX)
add_compile_definitions(_GLFW_X11)
add_library(GLFW "${LIB_PATH}/posix_thread.c" 
                 "${LIB_PATH}/posix_time.c"
                 "${LIB_PATH}/xkb_unicode.c" 
                 "${LIB_PATH}/x11_init.c"
                 "${LIB_PATH}/x11_monitor.c" 
                 "${LIB_PATH}/x11_window.c"
                 "${LIB_PATH}/glx_context.c" 
                 "${LIB_PATH}/linux_joystick.c")
ENDIF()

target_include_directories(GLFW PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
