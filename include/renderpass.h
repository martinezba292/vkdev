#ifndef __VK_RENDERPASS_H__
#define __VK_RENDERPASS_H__ 1

#include "common_def.h"

namespace vkdev {
  class SwapChain;
  class Device;
  class RenderPass {
    public:
      RenderPass();
      ~RenderPass();
  
      bool createRenderPass(const Device& device, const SwapChain& attachments);
      bool destroyRenderPass();
      const VkRenderPass& getHandle() const;
      const DeviceHandle* getDevice() const;

    private:
      VkRenderPass renderPass_;
      const DeviceHandle* deviceOwner_;
  };
}

#endif