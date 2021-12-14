#include "vksurface.h"


vkdev::Surface::Surface() {

}


vkdev::Surface::~Surface() {

}

vkdev::Surface::getHandle() {
  return surface_;
}


//TODO: Initialize window
vkdev::Surface::createSurface() {
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VkWin32SurfaceCreateInfoKHR surface_create_info{};
#elif defined VK_USE_PLATFORM_XLIB_KHR
    VkXlibSurfaceCreateInfoKHR surface_create_info{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
    surface_create_info.
#elif defined VK_USE_PLATFORM_XCB_KHR
    VkXcbSurfaceCreateInfoKHR surface_create_info{};
#endif
}