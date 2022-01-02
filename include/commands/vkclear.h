#ifndef __VK_CLEAR_H__
#define __VK_CLEAR_H__ 1

#include "vkcommand.h"
#include "vulkan/vulkan.h"
#include "vec4.hpp"

namespace vkdev {
  class ClearCmd : public Command {
    public:
      ClearCmd(const float* rgba, const VkImage& render_img, const uint32_t& queue_family);
      ClearCmd(const ClearCmd& cmd);
      ~ClearCmd();
      void record(const VkCommandBuffer& buffer) const override;

    private:
      VkClearColorValue clearColor_;
      const VkImage* clearImg_;
      uint32_t queueFamily_;
      
  };
}

#endif