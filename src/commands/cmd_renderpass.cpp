#include "commands/cmd_renderpass.h"
#include "renderpass.h"
#include "framebuffer.h"
#include <array>


vkdev::RenderpassBeginCmd::RenderpassBeginCmd(const RenderPass& renderpass, 
                                              const Framebuffer& framebuffer,
                                              const float* rgba) {
  type_ = CommandType_BeginRenderPass;
  attachmentRef_ = &framebuffer;
  renderPass_ = &renderpass.getHandle();
  clearColor_ = {rgba[0], rgba[1], rgba[2], rgba[3]};
}

vkdev::RenderpassBeginCmd::~RenderpassBeginCmd() {
  attachmentRef_ = nullptr; 
  renderPass_ = nullptr;
}

void vkdev::RenderpassBeginCmd::record(const VkCommandBuffer& buffer) const {

  std::array<VkClearValue, 2> clear;
  clear[0].color = clearColor_;
  clear[1].depthStencil = {1.0f, 0};
    
  auto size = attachmentRef_->getFramebufferSize();
  VkRenderPassBeginInfo renderpass_begin_i {
    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    nullptr,
    *renderPass_,
    attachmentRef_->getHandle(),
    {
      {
        0, 0
      },
      {
        static_cast<uint32_t>(size.x_), 
        static_cast<uint32_t>(size.y_)
      }
    },
    static_cast<uint32_t>(clear.size()),
    &clear[0]
  };

  vkCmdBeginRenderPass(buffer, &renderpass_begin_i, VK_SUBPASS_CONTENTS_INLINE);
  
}