#ifndef __VKWINDOW_H__
#define __VKWINDOW_H__ 1

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

#define WINDOW_W 800
#define WINDOW_H 600

namespace vkdev {
class Instance;
  class Window {
    public:
      Window(Instance&);
      ~Window();
      bool createWindow();
      bool destroyWindow();
      bool windowShouldClose() const;
      const VkSurfaceKHR& getSurface() const;
    private:
      GLFWwindow* window_;
      Instance* app_instance;
      VkSurfaceKHR surface_;
    };
}

#endif