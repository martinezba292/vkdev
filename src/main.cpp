#include "vkloader.h"
#include "vkinstance.h"
#include "vkdevice.h"
#include "window.h"
#include "vkswapchain.h"
#include "command_buffer.h"
#include "commands/vkclear.h"
#include <iostream>


#define FRAME_IMAGES 3

int main() {
  vkdev::PrepareVulkan();
  vkdev::Instance app_instance("VulkanTest");
  vkdev::Window app_window(app_instance);
  app_window.createWindow();
  vkdev::Device app_device(app_instance, app_window);
  vkdev::SwapChain app_swapchain;
  app_swapchain.createSwapchain(app_device, app_window, FRAME_IMAGES);
  
  vkdev::CommandBuffer cmd_buffer(FRAME_IMAGES);
  cmd_buffer.createCommandBuffer(&app_device);
  
  float colors[4] = {0.2f, 0.8f, 0.2f, 0.0f};
  std::vector<VkImage> swapchain_img;
  app_swapchain.getSwapchainImages(swapchain_img);
  for (size_t i = 0; i < FRAME_IMAGES; i++) {
      vkdev::ClearCmd clear(&colors[0], swapchain_img[i], app_device.getDeviceQueue()->queueFamily_);
      cmd_buffer.submitCommand(clear, i);
  }
  cmd_buffer.recordCommandBuffers();
  
  int j = 0;
  while(!app_window.windowShouldClose()) {
      app_swapchain.presentNextImage(cmd_buffer);
      //std::cout << j << std::endl;
  }
  
  cmd_buffer.destroyCommandBuffers();
  app_swapchain.~SwapChain();
  app_window.~Window();
  app_device.~Device();
  app_instance.~Instance();
  vkdev::CloseVulkan();

  return 0;
}
