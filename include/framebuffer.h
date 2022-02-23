#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__ 1

#include "common_def.h"
#include <vector>
#include "glm.hpp"

namespace vkdev {
  class RenderPass;
  class Framebuffer {
    public:
      Framebuffer();
      ~Framebuffer();

      bool createFramebuffer(const ImageHandle& src,
                             const ImageFormat& format, 
                             const RenderPass& attach_ref,
                             const uint32_t width,
                             const uint32_t height);
      const VkFramebuffer& getHandle() const;
      glm::uvec2 getFramebufferSize() const;
      bool destroyFramebuffer();
      
    private:
      VkFramebuffer framebuffer_;
      ViewHandle framebufferView_;
      const DeviceHandle* deviceOwner_;
      glm::uvec2 fbSize_;
  };
}

#endif