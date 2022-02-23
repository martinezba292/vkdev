#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__ 1

#include "common_def.h"
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
      int32_t getImageFormat() const;
      bool presentNextImage(const CommandBuffer& commands);

    private:
      VkSwapchainKHR swapchain_;
      VkFormat imageFormat_;
      VkSemaphore renderingFinished_;
      const Device* deviceOwner_;
  };
}


#endif