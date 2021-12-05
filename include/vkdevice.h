#ifndef __VK_DEVICE__ 
#define __VK_DEVICE__ 1
#include "vulkan/vulkan.h"


namespace vkdev {
  class Device {
  public:
    Device(VkInstance& instance);
    ~Device();



  private:
    bool createDevice(VkInstance& instance);
    bool checkPhysicalDeviceProperties(VkPhysicalDevice device, uint32_t queue_family);

    VkDevice device_;

  };
}

#endif