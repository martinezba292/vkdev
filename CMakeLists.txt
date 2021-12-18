cmake_minimum_required(VERSION 3.10)

#project name
project(VkDev VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

configure_file("${INC_PATH}/vkdevconfig.h.in" "${INC_PATH}/vkdevconfig.h")

add_executable(VkDev "${SRC_PATH}/main.cpp" 
                     "${SRC_PATH}/vkdevice.cpp" 
                     "${SRC_PATH}/vkfunctions.cpp" 
                     "${SRC_PATH}/vkinstance.cpp" 
                     "${SRC_PATH}/vkloader.cpp"
                     "${SRC_PATH}/window.cpp")
                     
#Library detection
INCLUDE(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_DLLIB)
IF(HAVE_DLLIB)
target_link_libraries(VkDev PUBLIC dl)
ENDIF()

CHECK_LIBRARY_EXISTS(X11 XkbKeycodeToKeysym "" XLIB)
IF(XLIB)
add_compile_definitions(VK_USE_PLATFORM_XLIB_KHR GLFW_EXPOSE_NATIVE_X11)
ENDIF()

#link GLFW
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
target_link_libraries(VkDev PUBLIC GLFW)

#Platform detection
IF (CMAKE_SYSTEM_NAME STREQUAL Windows)
add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
ENDIF()

target_include_directories(VkDev PUBLIC 
                            "${INC_PATH}" 
					        "${CMAKE_CURRENT_SOURCE_DIR}/deps/vulkan/Include"
                            "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include/GLFW")