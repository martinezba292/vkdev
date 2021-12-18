/*
Author: Manuel Martinez Barreiro
E-mail: martinezba292@gmail.com
*/

#ifndef __VK_DEVICE__ 
#define __VK_DEVICE__ 1
#include "vulkan/vulkan.h"

enum QueueFamilySupport {
  SUPPORT_GRAPHICS_OP = 0x0000001,
  SUPPORT_PRESENT_OP = 0x0000002,
  SUPPORT_TRANSFER_OP = 0x0000004,
  SUPPORT_COMPUTE_OP = 0x0000008,
};

struct Queue {
  uint16_t queueFamily_;
  uint16_t queueCount_;
  uint16_t index_;
  VkQueue queue_;
};

namespace vkdev {
  class Instance;
  class Window;
  class Device {
  public:
    Device();
    Device(Instance& instance, Window& window);
    ~Device();
    bool createDevice(Instance& instance, Window& window);

  private:
    VkDevice device_;

    Queue deviceQueue_;

    
    //VkQueue deviceQueue_;

  };
}

#endif