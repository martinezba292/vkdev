#ifndef __VK_SURFACE__
#define __VK_SURFACE__ 1

#include "vulkan/vulkan.h"

namespace vkdev {
  class Surface {
    public:
      Surface();
      ~Surface();
      
      bool createSurface();
      VkSurfaceKHR& getHandle();
    private:
      VkSurfaceKHR surface_;

  };
}

#endif