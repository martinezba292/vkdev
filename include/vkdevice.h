#ifndef __VK_DEVICE__ 
#define __VK_DEVICE__ 1
#include "vulkan/vulkan.h"


namespace vkdev {
  class Device {
  public:
    Device();
    ~Device();

    bool initDevice();

  private:
    VkDevice device_;

  };
}

#endif