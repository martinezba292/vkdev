#include "window.h"
#include "vkinstance.h"
#include <iostream>

#ifdef VK_USE_PLATFORM_XLIB_KHR
#include "vkfunctions.h"
#include "glfw3native.h"
#endif


vkdev::Window::Window(Instance& vkinstance) {
  window_ = nullptr;
  app_instance = &vkinstance;
}

vkdev::Window::~Window() {
  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
    window_ = nullptr;
  }
}

VkSurfaceKHR vkdev::Window::getSurface() {
  return surface_;
}


bool vkdev::Window::createWindow(const int32_t width, const int32_t height) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(width, height, "VkDev", nullptr, nullptr);
  VkResult r;
  #if defined (VK_USE_PLATFORM_WIN32_KHR) || (defined VK_USE_PLATFORM_XCB_KHR)
    r = glfwCreateWindowSurface(app_instance->get(), window_, nullptr, &surface_);
  #elif defined VK_USE_PLATFORM_XLIB_KHR
    VkXlibSurfaceCreateInfoKHR surface_info{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
    surface_info.dpy = glfwGetX11Display();
    surface_info.window = glfwGetX11Window(window_);
    r = vkCreateXlibSurfaceKHR(app_instance->get(), &surface_info, nullptr, &surface_);
  #endif
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create window surface " << r;
    return false;
  }

  return true;
}

bool vkdev::Window::windowShouldClose() const {
  glfwPollEvents();
  int state = glfwGetKey(window_, GLFW_KEY_ESCAPE);
  return (state == GLFW_PRESS);
}