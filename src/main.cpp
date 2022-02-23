#include "vkloader.h"
#include "vkinstance.h"
#include "vkdevice.h"
#include "window.h"
#include "vkswapchain.h"
#include "command_buffer.h"
#include "renderpass.h"
#include "framebuffer.h"
#include "pipeline_resources.h"
#include "vkbuffer.h"
#include "commands/cmd_renderpass.h"
#include "commands/cmd_bindpipeline.h"
#include "commands/cmd_draw.h"
#include "commands/vkclear.h"
#include <iostream>
#include <chrono>


//Returns the difference between 2 times
std::chrono::duration<double> GetElapsedTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& start) {
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = (end - start);
  return elapsed;
}

//Calculate the total average time from a given vector and shows it on screen
void AverageElapsedTime(const std::vector<std::chrono::duration<double>>& v) {
  double t;
  for (auto measure : v) {
    t += measure.count();
  }
  std::cout << "Average elapsed time from " << v.size() << 
  " samples: " << static_cast<double>(t / v.size()) << std::endl;
}

  //Swapchain images
  const uint32_t FRAME_IMAGES = 3;
  const uint32_t measure_samples = 3000;

int main() {

  //Application starts
  auto start = std::chrono::high_resolution_clock::now();
  std::cout << "Start!!" << std::endl;

  //Loads Vulkan library and functions
  vkdev::PrepareVulkan();

  //Create vulkan instance
  vkdev::Instance app_instance("VulkanTest");

  //Create GLFW window
  vkdev::Window app_window(app_instance);
  app_window.createWindow();

  //Create Vulkan physical and logical device
  vkdev::Device app_device(app_instance, app_window);

  //Creates the swapchain with a given number of image views
  vkdev::SwapChain app_swapchain;
  app_swapchain.createSwapchain(app_device, app_window, FRAME_IMAGES);

  //Creates a renderpass based on a swapchain attachment
  vkdev::RenderPass renderpass;
  renderpass.createRenderPass(app_device, app_swapchain);

  //Get swapchain images in order to create framebuffer
  std::vector<ImageHandle> swapchain_img;
  app_swapchain.getSwapchainImages(swapchain_img);
  ImageFormat format = static_cast<ImageFormat>(app_swapchain.getImageFormat());
  std::vector<vkdev::Framebuffer> framebuffer(FRAME_IMAGES);
  uint32_t w = 400;
  uint32_t h = 400;
  

  //Creates a framebuffer based on an image format, renderpass and a size
  for (size_t i = 0; i < FRAME_IMAGES; i++) {
    framebuffer[i].createFramebuffer(swapchain_img[i], format, renderpass, WINDOW_W, WINDOW_H);
  }
  
  //Creates command pool and allocate command buffers
  vkdev::CommandBuffer cmd_buffer(FRAME_IMAGES);
  cmd_buffer.createCommandBuffer(&app_device);

  float vertex_data[] = {
   -0.7f, -0.7f, 0.0f,
    1.0f, 0.0f, 0.0f,

   -0.7f, 0.7f, 0.0f,
    0.0f, 1.0f, 0.0f,

    0.7f, -0.7f, 0.0f,
    0.0f, 0.0f, 1.0f,
  
    0.7f, 0.7f, 0.0f,
    0.3f, 0.3f, 0.3f,
  };

  uint32_t elements = sizeof(vertex_data) / sizeof(float);

  vkdev::VertexBuffer buffer;
  buffer.loadData(vertex_data, elements);

  //Shader paths
  std::vector<std::string> shaders {
    "../src/shaders/spirv/test_vert.spv",
    "../src/shaders/spirv/test_frag.spv"    
  };

  //Create pipeline
  vkdev::PipelineResources pipeline;
  bool r = pipeline.createGraphicsPipeline(app_device, renderpass, VertexParams_3V_3N,
                                           WINDOW_W, WINDOW_H, 
                                           shaders[0].c_str(), shaders[1].c_str());
  
  //Clear color
  float colors[4] = {0.0f, 0.2f, 0.8f, 0.0f};

  //Command record for rendering
  for (size_t i = 0; i < FRAME_IMAGES; i++) {
    vkdev::RenderpassBeginCmd begin_renderpass(renderpass, 
                                                framebuffer[i], 
                                                colors);
    vkdev::BindPipelineCmd bind_pipeline(pipeline);
    vkdev::DrawCmd draw(3, 1);
    vkdev::RenderpassEndCmd end_renderpass;
    cmd_buffer.submitCommand(begin_renderpass, i);
    cmd_buffer.submitCommand(bind_pipeline, i);
    cmd_buffer.submitCommand(draw, i);
    cmd_buffer.submitCommand(end_renderpass, i);
  }
  cmd_buffer.recordCommandBuffers();

  //Shows vulkan setup time
  auto t = GetElapsedTime(start);
  std::cout << "Setup took: " << t.count() << " seconds" << std::endl;

  //Time samples
  std::vector<std::chrono::duration<double>> samples(measure_samples);
  uint32_t j = 0;

  //RENDER LOOP
  while(!app_window.windowShouldClose()) {
    start = std::chrono::high_resolution_clock::now();
      app_swapchain.presentNextImage(cmd_buffer);
    samples[j] = GetElapsedTime(start);
    ++j %= measure_samples;
  }

  //Shows average render time
  AverageElapsedTime(samples);

  //Clean up
  std::cout << "Cleaning up..." << std::endl;
  pipeline.destroyPipelineResources();
  cmd_buffer.destroyCommandBuffers();
  for (size_t i = 0; i < FRAME_IMAGES; i++) {
    framebuffer[i].destroyFramebuffer();
  }
  framebuffer.clear();

  renderpass.destroyRenderPass();
  app_swapchain.~SwapChain();

  app_device.destroyDevice();
  app_window.destroyWindow();
  app_instance.destroyInstance();
  vkdev::CloseVulkan();

  std::cout << "End application" << std::endl;

  return 0;
}
