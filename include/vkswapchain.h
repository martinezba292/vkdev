#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__ 1

#include "vulkan/vulkan.h"
#include <vector>

namespace vkdev {
  class Window;
  class Device;
  class CommandBuffer;
  class SwapChain {
    public:
      SwapChain();
      ~SwapChain();

      bool createSwapchain(const Device& device, const Window& window, uint32_t n_images);
      bool getSwapchainImages(std::vector<VkImage>& img);
      bool presentNextImage(const CommandBuffer& commands);

    private:
      VkSwapchainKHR swapchain_;
      VkSemaphore renderingFinished_;
      const Device* deviceOwner_;
  };
}


#endif