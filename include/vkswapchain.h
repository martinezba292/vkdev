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
      bool destroySwapchain();
      bool getSwapchainImages(std::vector<VkImage>& img);
      const std::vector<ViewHandle>& getSwapchainImageViews() const;
      const VkSwapchainKHR& getHandle() const;
      ImageFormat getImageFormat() const;
      //bool presentNextImage(const CommandBuffer& commands);

    private:
      VkSwapchainKHR swapchain_;
      VkFormat imageFormat_;
      std::vector<ViewHandle> scImageView_;
      //const Device* deviceOwner_;
  };
}


#endif