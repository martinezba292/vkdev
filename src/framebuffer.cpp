#include "framebuffer.h"
#include "vkdevice.h"
#include "vkhelpers.h"
#include "renderpass.h"
#include "vkfunctions.h"
#include "internal_resources.h"
#include <iostream>

vkdev::Framebuffer::Framebuffer() {
  framebuffer_ = VK_NULL_HANDLE;
  framebufferView_ = VK_NULL_HANDLE;
  // depthImg_ = VK_NULL_HANDLE;
  // depthMem_ = VK_NULL_HANDLE;
  // depthView_ = VK_NULL_HANDLE;
  //deviceOwner_ = nullptr;
  fbSize_ = lau::IVector2D(0, 0);
}

vkdev::Framebuffer::~Framebuffer() {
    destroyFramebuffer();
}

const VkFramebuffer& vkdev::Framebuffer::getHandle() const {

  return framebuffer_;
}

lau::IVector2D vkdev::Framebuffer::getFramebufferSize() const {
  return fbSize_;
}

bool vkdev::Framebuffer::softDestroy() {
  if (framebuffer_ != VK_NULL_HANDLE) {
    const DeviceHandle& deviceh = ResourceManager::GetResources()->device_;
    vkDestroyFramebuffer(deviceh, framebuffer_, nullptr);
    framebuffer_ = VK_NULL_HANDLE;
    if (framebufferView_ != VK_NULL_HANDLE) {
      vkDestroyImageView(deviceh, framebufferView_, nullptr);
      framebufferView_ = VK_NULL_HANDLE;
    }
    return true;
  }
  return false;
}


bool vkdev::Framebuffer::destroyFramebuffer() {
  if (!softDestroy()) {
    return false;
  }
  //destroyDepthResources();
  return true;
}


bool vkdev::Framebuffer::createFramebuffer(const ImageHandle& src,
                                           const ImageFormat& format,
                                           const RenderPass& attach_ref,
                                           const uint32_t width,
                                           const uint32_t height) {

  const DeviceHandle* deviceOwner_ = attach_ref.getDevice();
  if (!deviceOwner_) {
    std::cout << "Couldn't create Framebuffer with given Renderpass" << std::endl;
    return false;
  }

  if (!createImageView(*deviceOwner_, format, src, framebufferView_)) {
      return false;
  }

  ViewHandle depth_view = ResourceManager::GetResources()->depthResources_->imgView_;
  std::array<ViewHandle, 2> attachments {framebufferView_, depth_view};
  VkFramebufferCreateInfo framebuffer_ci {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebuffer_ci.renderPass = attach_ref.getHandle();
  framebuffer_ci.attachmentCount = attachments.size();
  framebuffer_ci.pAttachments = &attachments[0];
  framebuffer_ci.layers = 1;
  framebuffer_ci.width = width;
  framebuffer_ci.height = height;

  fbSize_.x_ = static_cast<int32_t>(width);
  fbSize_.y_ = static_cast<int32_t>(height);

  VkResult r = vkCreateFramebuffer(*deviceOwner_, &framebuffer_ci, nullptr, &framebuffer_);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create Framebuffer" << std::endl;
    return false;
  }

  return true;
}


bool vkdev::Framebuffer::createFramebuffer(const ViewHandle& color_attachment,
                                           const RenderPass& attach_ref,
                                           const uint32_t width,
                                           const uint32_t height) {
  if (color_attachment == VK_NULL_HANDLE) {
    std::cout << "Invalid image view handle for framebuffer creation" << std::endl;
    return false;
  }
  
  softDestroy();
  ViewHandle depth_view = ResourceManager::GetResources()->depthResources_->imgView_;
  std::array<ViewHandle, 2> img_attachments{color_attachment, depth_view};
  VkFramebufferCreateInfo framebuffer_ci {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebuffer_ci.renderPass = attach_ref.getHandle();
  framebuffer_ci.attachmentCount = static_cast<uint32_t>(img_attachments.size());
  framebuffer_ci.pAttachments = &img_attachments[0];
  framebuffer_ci.layers = 1;
  framebuffer_ci.width = width;
  framebuffer_ci.height = height;

  fbSize_.x_ = static_cast<int32_t>(width);
  fbSize_.y_ = static_cast<int32_t>(height);

  const DeviceHandle& deviceh = ResourceManager::GetResources()->device_;
  VkResult r = vkCreateFramebuffer(deviceh, &framebuffer_ci, nullptr, &framebuffer_);

  if (r != VK_SUCCESS) {
    std::cout << "Failed to create Framebuffer" << std::endl;
    return false;
  }

  return true;
}


// bool vkdev::Framebuffer::createDepthResources() {
//   VkFormat format = findSupportedFormats({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
//                                           VK_IMAGE_TILING_OPTIMAL,
//                                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

//   createImage(depthImg_, fbSize_.x, fbSize_.y, VK_IMAGE_TYPE_2D, format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
//   allocateImageMemory(depthMem_, depthImg_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

//   const VkDevice device = ResourceManager::GetResources()->device_;
//   createImageView(device, format, depthImg_, depthView_, VK_IMAGE_ASPECT_DEPTH_BIT);

//   return true;
// }


// void vkdev::Framebuffer::destroyDepthResources() {
//   DeviceHandle dev = ResourceManager::GetResources()->device_;
//   if (depthView_ != VK_NULL_HANDLE) {
//     vkDestroyImageView(dev, depthView_, nullptr);
//     depthView_ = VK_NULL_HANDLE;
//   }

//   if (depthImg_ != VK_NULL_HANDLE) {
//     vkDestroyImage(dev, depthImg_, nullptr);
//     vkFreeMemory(dev, depthMem_, nullptr);
//     depthImg_ = VK_NULL_HANDLE;
//     depthMem_ = VK_NULL_HANDLE;
//   }
// }