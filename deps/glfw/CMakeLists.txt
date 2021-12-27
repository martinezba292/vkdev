#CMake config for GLFW files

set(LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/src")

set(COMMON_FILES  "${LIB_PATH}/context.c"
                  "${LIB_PATH}/init.c"
                  "${LIB_PATH}/input.c"
                  "${LIB_PATH}/monitor.c"
                  "${LIB_PATH}/window.c"
                  "${LIB_PATH}/vulkan.c"
                  "${LIB_PATH}/egl_context.c"
                  "${LIB_PATH}/osmesa_context.c")

#add_compile_definitions(GLFW_INCLUDE_VULKAN)
                  
IF (CMAKE_SYSTEM_NAME STREQUAL Windows)
add_compile_definitions(_GLFW_WIN32 _GLFW_WGL)
add_library(GLFW "${COMMON_FILES}"
                 "${LIB_PATH}/win32_init.c" 
                 "${LIB_PATH}/win32_joystick.c"
                 "${LIB_PATH}/win32_monitor.c"
                 "${LIB_PATH}/win32_time.c"
                 "${LIB_PATH}/win32_thread.c"
                 "${LIB_PATH}/win32_window.c"
                 "${LIB_PATH}/wgl_context.c")

ELSEIF (CMAKE_SYSTEM_NAME STREQUAL Linux)
add_compile_definitions(_GLFW_X11)
add_library(GLFW "${COMMON_FILES}"
                 "${LIB_PATH}/posix_thread.c" 
                 "${LIB_PATH}/posix_time.c"
                 "${LIB_PATH}/xkb_unicode.c" 
                 "${LIB_PATH}/x11_init.c"
                 "${LIB_PATH}/x11_monitor.c" 
                 "${LIB_PATH}/x11_window.c"
                 "${LIB_PATH}/glx_context.c" 
                 "${LIB_PATH}/linux_joystick.c")

CHECK_LIBRARY_EXISTS(pthread pthread_key_create "" PTHREAD_LIB)
IF(PTHREAD_LIB)
    target_link_libraries(GLFW PUBLIC pthread)
ENDIF()

CHECK_LIBRARY_EXISTS(X11 XkbKeycodeToKeysym "" XLIB)
IF(XLIB)
    target_link_libraries(GLFW PUBLIC X11)
ENDIF()
ENDIF()

target_include_directories(GLFW PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
