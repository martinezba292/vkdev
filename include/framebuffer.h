#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__ 1

#include "common_def.h"
// #include <vector>
// #include "glm.hpp"
#include "ivector2.hpp"

namespace vkdev {
  struct DepthResources {
    VkImage img_;
    ViewHandle imgView_; 
  };
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
      
      bool createFramebuffer(const ViewHandle& color_attachment,
                             const RenderPass& attach_ref,
                             const uint32_t width,
                             const uint32_t height);

      //bool createDepthResources();
      //void destroyDepthResources();
      const VkFramebuffer& getHandle() const;
      lau::IVector2D getFramebufferSize() const;
      bool destroyFramebuffer();
      bool softDestroy();
      
    private:
      VkFramebuffer framebuffer_;
      ViewHandle framebufferView_;
      lau::IVector2D fbSize_;
      //VkImage depthImg_;
      //ViewHandle depthView_; 
      //VkDeviceMemory depthMem_;
      //glm::uvec2 fbSize_;
      //const DeviceHandle* deviceOwner_;
  };
}

#endif