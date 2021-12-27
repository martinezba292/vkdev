/*
Author: Manuel Martinez Barreiro
E-mail: martinezba292@gmail.com
*/

#ifndef __VKDEVICE_H__ 
#define __VKDEVICE_H__ 1
#include "vulkan/vulkan.h"
#include <memory>

typedef uint8_t QueueFamilyFlags;

enum QueueFamilySupport {
  SUPPORT_GRAPHICS_OP = 0x00000001,
  SUPPORT_PRESENT_OP = 0x00000002,
  SUPPORT_TRANSFER_OP = 0x00000004,
  SUPPORT_COMPUTE_OP = 0x00000008,
};

namespace vkdev {
  class Instance;
  class Window;
  class Device {
  public:
  struct DeviceQueue {
    VkPhysicalDevice currentDevice_;
    VkSemaphore imageAvailableSemaphore_;
    int16_t queueFamily_;
    int16_t queueCount_;
    VkQueue queue_;
  };
    Device();
    Device(const Instance& instance, const Window& window);
    ~Device();

    bool createDevice(const Instance& instance, const Window& window);
    const VkDevice& getDeviceHandle() const;
    const VkPhysicalDevice& getCurrentPhysicalDevice() const;
    const DeviceQueue* getDeviceQueue() const;
    uint32_t ratePhysicalDeviceProperties(const VkPhysicalDevice& device, const Window& window_surface);
   
   /**
    * @brief Get Queue family with support for operations passed as flags
    * 
    * @param device from which we actually get the queue 
    * @param support required operations for the queue 
    * @param min_queue 
    * @param window_handle only if present support is required
    * @return true if a queue is found
    */
    std::shared_ptr<DeviceQueue> findQueueFamilyWithProperties(const VkPhysicalDevice& device,
                                                               const QueueFamilyFlags support,
                                                               const uint16_t min_queue,
                                                               const Window* window_handle = nullptr);
  private:
    VkPhysicalDevice physDevice_;
    VkDevice device_;
    std::shared_ptr<DeviceQueue> deviceQueue_;
  };
}

#endif