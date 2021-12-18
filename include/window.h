#ifndef __VKWINDOW_H__
#define __VKWINDOW_H__ 1

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

namespace vkdev {
class Instance;
  class Window {
    public:
      Window(Instance&);
      ~Window();
      bool createWindow(const int32_t width, const int32_t height);
      bool windowShouldClose() const;
      VkSurfaceKHR getSurface();
    private:
      GLFWwindow* window_;
      Instance* app_instance;
      VkSurfaceKHR surface_;


    };
}

#endif