#include "vkswapchain.h"
#include "vkdevice.h"
#include "window.h"
#include "vkfunctions.h"
#include "command_buffer.h"
#include <iostream>
#include <vector>

/***************************SWAPCHAIN SELECTION PROPERTIES FUNCTIONS**************************/

uint32_t GetSwapChainImageCount(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t imgs_number) {

  return std::min(imgs_number, capabilities.maxImageCount);
}

/////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR GetSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& surface_formats) {
  //If format is undefined means that every format is suitable
  if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
    return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR};

  for (const VkSurfaceFormatKHR& format: surface_formats) {
    if (format.format == VK_FORMAT_R8G8B8A8_UNORM)
      return format;
  }

  return surface_formats[0];
}

/////////////////////////////////////////////////////////////////////

VkExtent2D GetSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width == -1) {
    return {WINDOW_W, WINDOW_H};
  }

  return capabilities.currentExtent;
}

/////////////////////////////////////////////////////////////////////

VkImageUsageFlags GetSwapchainFlags(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    return (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  }

  std::cout << "Image transfer operations isn't supported by the swapchain";
  return static_cast<VkImageUsageFlags>(-1);
}

/////////////////////////////////////////////////////////////////////

VkSurfaceTransformFlagBitsKHR GetSwapchainTransform(const VkSurfaceCapabilitiesKHR& capabilities) {
  //Don't want any transformations so if the identity is available the app will pick it up
  if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }

  return capabilities.currentTransform;
}

/////////////////////////////////////////////////////////////////////

VkPresentModeKHR GetPresentMode(const std::vector<VkPresentModeKHR>& present_modes) {
  //In mailbox mode new image replace old one
  //Always most recent image in queue
  for (const VkPresentModeKHR& present : present_modes) {
    if (present == VK_PRESENT_MODE_MAILBOX_KHR) {
      return present;
    }
  }


  for (const VkPresentModeKHR& present : present_modes) {
    if (present == VK_PRESENT_MODE_FIFO_KHR) {
      return present;
    }
  }

  std::cout << "FIFO present mode is not supported by swapchain" << std::endl;
  return static_cast<VkPresentModeKHR>(-1);
}

/***********************************************************************************************/


vkdev::SwapChain::SwapChain() {
  swapchain_ = VK_NULL_HANDLE;
  renderingFinished_ = VK_NULL_HANDLE;
  imageFormat_ = VK_FORMAT_UNDEFINED;
  deviceOwner_ = nullptr;
}

/***********************************************************************************************/

vkdev::SwapChain::~SwapChain() {
  if (swapchain_ != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(deviceOwner_->getDeviceHandle());
    vkDestroySemaphore(deviceOwner_->getDeviceHandle(), renderingFinished_, nullptr);
    vkDestroySwapchainKHR(deviceOwner_->getDeviceHandle(), swapchain_, nullptr);
    swapchain_ = VK_NULL_HANDLE;
  }
}

/***********************************************************************************************/

bool vkdev::SwapChain::createSwapchain(const Device& device, const Window& window, uint32_t n_images) {

deviceOwner_ = &device;
/******************************SURFACE CAPABILITIES***************************************/
  VkSurfaceCapabilitiesKHR capabilities;
  VkResult r = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.getCurrentPhysicalDevice(), window.getSurface(), &capabilities);
  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred getting surface support capabilities" << std::endl;
    return false;
  }
  

/******************************SURFACE FORMATS***************************************/
  uint32_t formats_count;
  r = vkGetPhysicalDeviceSurfaceFormatsKHR(device.getCurrentPhysicalDevice(), 
                                           window.getSurface(), 
                                           &formats_count, 
                                           nullptr);
  if (r != VK_SUCCESS || formats_count == 0) {
    std::cout << "Error ocurred during presentation surface formats enumeration!" << std::endl;
    return false;
  }

  std::vector<VkSurfaceFormatKHR> formats(formats_count);
  r = vkGetPhysicalDeviceSurfaceFormatsKHR(device.getCurrentPhysicalDevice(), 
                                           window.getSurface(), 
                                           &formats_count, 
                                           &formats[0]);
  if (r != VK_SUCCESS) {
    std::cout << "Error ocurred during presentation surface formats enumeration!" << std::endl;
    return false;
  }


/******************************PRESENT MODES***************************************/
  uint32_t present_mode_count;
  r = vkGetPhysicalDeviceSurfacePresentModesKHR(device.getCurrentPhysicalDevice(), 
                                                window.getSurface(), 
                                                &present_mode_count, 
                                                nullptr);
  if (r != VK_SUCCESS) {
    std::cout << "Error ocurred during presentation modes enumeration!" << std::endl;
    return false;
  }

  std::vector<VkPresentModeKHR> present_modes(present_mode_count);
  r = vkGetPhysicalDeviceSurfacePresentModesKHR(device.getCurrentPhysicalDevice(), 
                                                window.getSurface(), 
                                                &present_mode_count, 
                                                &present_modes[0]);
  if (r != VK_SUCCESS) {
    std::cout << "Error ocurred during presentation modes enumeration!" << std::endl;
    return false;
  }

/////SWAPCHAIN CREATE INFO
  VkImageUsageFlags usage = GetSwapchainFlags(capabilities);
  VkPresentModeKHR present = GetPresentMode(present_modes);

  if (static_cast<int32_t>(usage) == -1 || static_cast<int32_t>(present) == -1) {
    return false;
  }

  VkSurfaceFormatKHR desired_format = GetSwapChainFormat(formats);
  imageFormat_ = desired_format.format;
  VkSwapchainCreateInfoKHR swch_create_info {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swch_create_info.surface = window.getSurface();
  swch_create_info.minImageCount = GetSwapChainImageCount(capabilities, n_images);
  swch_create_info.imageFormat = imageFormat_;
  swch_create_info.imageColorSpace = desired_format.colorSpace;
  swch_create_info.imageExtent = GetSwapChainExtent(capabilities);
  swch_create_info.imageArrayLayers = 1;
  swch_create_info.imageUsage = usage;
  swch_create_info.preTransform = GetSwapchainTransform(capabilities);
  swch_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swch_create_info.presentMode = present;
  swch_create_info.clipped = VK_TRUE;

  r = vkCreateSwapchainKHR(device.getDeviceHandle(), &swch_create_info, nullptr, &swapchain_);

  VkSemaphoreCreateInfo semaphore_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  vkCreateSemaphore(device.getDeviceHandle(), &semaphore_info, nullptr, &renderingFinished_);

  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred during swapchain creation" << std::endl;
    return false;
  }

  return true;
}


bool vkdev::SwapChain::getSwapchainImages(std::vector<VkImage>& img) {
  uint32_t image_count;
  VkResult r = vkGetSwapchainImagesKHR(deviceOwner_->getDeviceHandle(), swapchain_, &image_count, nullptr);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't get swapchain image number" << std::endl;
    return false;
  }
  
  img.resize(image_count);
  r = vkGetSwapchainImagesKHR(deviceOwner_->getDeviceHandle(), swapchain_, &image_count, &img[0]);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't get swapchain images" << std::endl;
    return false;
  }
  
  return true;
}

int32_t vkdev::SwapChain::getImageFormat() const{
  return imageFormat_;
}


bool vkdev::SwapChain::presentNextImage(const CommandBuffer& commands) {
  uint32_t image_index;
  const Device::DeviceQueue* queue = deviceOwner_->getDeviceQueue();
  VkResult r = vkAcquireNextImageKHR(deviceOwner_->getDeviceHandle(), 
                                     swapchain_, UINT64_MAX, 
                                     queue->imageAvailableSemaphore_,
                                     VK_NULL_HANDLE, &image_index);
  
  if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) {
    std::cout << "Problem ocurred during swapchain acquisition!" << std::endl;
    return false;
  }

  VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &queue->imageAvailableSemaphore_;
  submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &commands.getCommandBuffer(image_index);
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &renderingFinished_;

  r = vkQueueSubmit(queue->queue_, 1, &submit_info, VK_NULL_HANDLE);
  if (r != VK_SUCCESS) {
    std::cout << "Error submitting command information to the queue";
    return false;
  }

  VkPresentInfoKHR present_info {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &renderingFinished_;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain_;
  present_info.pImageIndices = &image_index;

  r = vkQueuePresentKHR(queue->queue_, &present_info);
  if (r != VK_SUCCESS) {
    std::cout << "Problem ocurred during image presentation!" << std::endl;
    return false;
  }

  return true;
}




