#include "framebuffer.h"
#include "vkdevice.h"
#include "vkhelpers.h"
#include "renderpass.h"
#include "vkfunctions.h"
#include <iostream>

vkdev::Framebuffer::Framebuffer() {
  framebuffer_ = VK_NULL_HANDLE;
  framebufferView_ = VK_NULL_HANDLE;
  deviceOwner_ = nullptr;
  fbSize_ = {0, 0};
}

vkdev::Framebuffer::~Framebuffer() {
    destroyFramebuffer();
}

const VkFramebuffer& vkdev::Framebuffer::getHandle() const {

  return framebuffer_;
}

glm::uvec2 vkdev::Framebuffer::getFramebufferSize() const {
  return fbSize_;
}

bool vkdev::Framebuffer::destroyFramebuffer() {
  if (framebuffer_ != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(*deviceOwner_, framebuffer_, nullptr);
    framebuffer_ = VK_NULL_HANDLE;
    vkDestroyImageView(*deviceOwner_, framebufferView_, nullptr);
    framebufferView_ = VK_NULL_HANDLE;
    deviceOwner_ = nullptr;
    return true;
  }

  return false;
}

bool vkdev::Framebuffer::createFramebuffer(const ImageHandle& src,
                                           const ImageFormat& format,
                                           const RenderPass& attach_ref,
                                           const uint32_t width,
                                           const uint32_t height) {
  deviceOwner_ = attach_ref.getDevice();
  if (!deviceOwner_) {
    std::cout << "Couldn't create Framebuffer with given Renderpass" << std::endl;
    return false;
  }

  if (!createImageView(*deviceOwner_, format, src, framebufferView_)) {
      return false;
  }

  VkFramebufferCreateInfo framebuffer_ci {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebuffer_ci.renderPass = attach_ref.getHandle();
  framebuffer_ci.attachmentCount = 1;
  framebuffer_ci.pAttachments = &framebufferView_;
  framebuffer_ci.layers = 1;
  framebuffer_ci.width = width;
  framebuffer_ci.height = height;

  fbSize_ = {width, height};

  VkResult r = vkCreateFramebuffer(*deviceOwner_, &framebuffer_ci, nullptr, &framebuffer_);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create Framebuffer" << std::endl;
    return false;
  }

  return true;
}

