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
      bool createWindow();
      bool destroyWindow();
      void input();
      bool windowShouldClose() const;
      const VkSurfaceKHR& getSurface() const;
      const GLFWwindow* getWindow() const;
      
    private:
      GLFWwindow* window_;
      Instance* app_instance;
      VkSurfaceKHR surface_;
      int32_t close_;
    };
}

#endif