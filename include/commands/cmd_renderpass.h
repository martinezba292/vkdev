#ifndef __RENDERPASS_CMD_H__
#define __RENDERPASS_CMD_H__ 1

#include "vkcommand.h"
#include "vkfunctions.h"

namespace vkdev {
  class RenderPass;
  class Framebuffer;
  class RenderpassBeginCmd : public Command{
    public:
      RenderpassBeginCmd(const RenderPass& renderpass, 
                         const Framebuffer& framebuffer,
                         const float* rgba);
      ~RenderpassBeginCmd();
      void record(const VkCommandBuffer& buffer) const override;

    private:
      const Framebuffer* attachmentRef_;
      const VkRenderPass* renderPass_;
      VkClearColorValue clearColor_;
  };

  class RenderpassEndCmd : public Command {
    public:
      inline RenderpassEndCmd(){type_ = CommandType_EndRenderPass;}
      ~RenderpassEndCmd(){}

      inline void record(const VkCommandBuffer& buffer) const override {
        vkCmdEndRenderPass(buffer);
      }
  };
}

#endif