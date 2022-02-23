#include "window.h"
#include "vkinstance.h"
#include <iostream>

#ifdef VK_USE_PLATFORM_XLIB_KHR
#include "vkfunctions.h"
#include "glfw3native.h"
#endif


vkdev::Window::Window(Instance& vkinstance) {
  window_ = nullptr;
  surface_ = VK_NULL_HANDLE;
  app_instance = &vkinstance;
}

vkdev::Window::~Window() {
  destroyWindow();
}

bool vkdev::Window::destroyWindow() {
  if (window_) {
    vkDestroySurfaceKHR(app_instance->get(), surface_, nullptr);
    glfwDestroyWindow(window_);
    
    glfwTerminate();
    app_instance = nullptr;
    surface_ = VK_NULL_HANDLE;
    window_ = nullptr;
    return true;
  }

  return false;
}

const VkSurfaceKHR& vkdev::Window::getSurface() const{
  return surface_;
}


bool vkdev::Window::createWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(WINDOW_W, WINDOW_H, "VkDev", nullptr, nullptr);
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