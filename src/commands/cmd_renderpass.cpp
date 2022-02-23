#include "commands/cmd_renderpass.h"
#include "renderpass.h"
#include "framebuffer.h"


vkdev::RenderpassBeginCmd::RenderpassBeginCmd(const RenderPass& renderpass, 
                                              const Framebuffer& framebuffer,
                                              const float* rgba) {
  type_ = CommandType_BeginRenderPass;
  attachmentRef_ = &framebuffer.getHandle();
  renderPass_ = &renderpass.getHandle();
  size_ = framebuffer.getFramebufferSize();
  clearColor_ = {rgba[0], rgba[1], rgba[2], rgba[3]};
}

vkdev::RenderpassBeginCmd::~RenderpassBeginCmd() {
  attachmentRef_ = nullptr; 
  renderPass_ = nullptr;
}

void vkdev::RenderpassBeginCmd::record(const VkCommandBuffer& buffer) const {

  VkClearValue clear {
    {clearColor_},
  };
  VkRenderPassBeginInfo renderpass_begin_i {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    nullptr,
    *renderPass_,
    *attachmentRef_,
    {
      {
        0, 0
      },
      {
      size_.x, size_.y
      }
    },
    1,
    &clear
  };

  vkCmdBeginRenderPass(buffer, &renderpass_begin_i, VK_SUBPASS_CONTENTS_INLINE);
  
}


// void vkdev::RenderpassEndCmd::record(const VkCommandBuffer& buffer) const {
//   vkCmdEndRenderPass(buffer);
// }