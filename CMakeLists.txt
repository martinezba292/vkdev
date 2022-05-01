cmake_minimum_required(VERSION 3.10)

#project name
project(VKDEV VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/src")
set(INC_PATH "${CMAKE_CURRENT_LIST_DIR}/include")

configure_file("${INC_PATH}/vkdevconfig.h.in" "${INC_PATH}/vkdevconfig.h")

set(LODE_PNG "${CMAKE_CURRENT_LIST_DIR}/deps/lodepng/lodepng.cpp")

set(IMGUI_PATH "${CMAKE_CURRENT_LIST_DIR}/deps/imgui")
set(IMGUI "${IMGUI_PATH}/imgui.cpp"
          "${IMGUI_PATH}/imgui_draw.cpp"
          "${IMGUI_PATH}/imgui_widgets.cpp"
          "${IMGUI_PATH}/imgui_demo.cpp"
          "${IMGUI_PATH}/imgui_tables.cpp"
          "${IMGUI_PATH}/backends/imgui_impl_glfw.cpp"
          "${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp")

add_library(VKDEV "${SRC_PATH}/vkdevice.cpp" 
                  "${SRC_PATH}/vkfunctions.cpp" 
                  "${SRC_PATH}/vkinstance.cpp" 
                  "${SRC_PATH}/vkloader.cpp"
                  "${SRC_PATH}/window.cpp"
                  "${SRC_PATH}/vkswapchain.cpp"
                  "${SRC_PATH}/command_buffer.cpp"
                  "${SRC_PATH}/renderpass.cpp"
                  "${SRC_PATH}/vkhelpers.cpp"
                  "${SRC_PATH}/framebuffer.cpp"
                  "${SRC_PATH}/pipeline_resources.cpp"
                  "${SRC_PATH}/vkbuffer.cpp"
                  "${SRC_PATH}/vktexture.cpp"
                  "${SRC_PATH}/pipeline_descriptor.cpp"
                  "${SRC_PATH}/UI.cpp"
                  "${SRC_PATH}/internal/internal_resources.cpp"
                  "${SRC_PATH}/commands/vkclear.cpp"
                  "${SRC_PATH}/commands/cmd_renderpass.cpp"
                  "${SRC_PATH}/commands/cmd_bindbuffer.cpp"
                  "${SRC_PATH}/commands/cmd_binddescriptor.cpp"
                  #"${SRC_PATH}/commands/cmd_drawUI.cpp"
                  "${LODE_PNG}"
                  "${IMGUI}")


                    
#Library detection
INCLUDE(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_DLLIB)
IF(HAVE_DLLIB)
target_link_libraries(VKDEV PUBLIC dl)
ENDIF()

CHECK_LIBRARY_EXISTS(X11 XkbKeycodeToKeysym "" XLIB)
IF(XLIB)
add_compile_definitions(VK_USE_PLATFORM_XLIB_KHR GLFW_EXPOSE_NATIVE_X11)
ENDIF()

#link GLFW
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/glfw")
target_link_libraries(VKDEV PUBLIC GLFW)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps/LinearAlgebraUtils")
target_link_libraries(VKDEV PUBLIC LAU)

#Platform detection
IF (CMAKE_SYSTEM_NAME STREQUAL Windows)
add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
ENDIF()

add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)


target_include_directories(VKDEV PUBLIC 
                            "${INC_PATH}" 
                            "${SRC_PATH}/internal"
					                  "${CMAKE_CURRENT_LIST_DIR}/deps/vulkan/Include"
                            "${CMAKE_CURRENT_LIST_DIR}/deps/glfw/include/GLFW"
                            "${CMAKE_CURRENT_LIST_DIR}/deps/LinearAlgebraUtils/include/"
                            #"${CMAKE_CURRENT_LIST_DIR}/deps/GLM/glm"
                            "${IMGUI_PATH}"
                            "${CMAKE_CURRENT_LIST_DIR}/deps/lodepng")