#include "UI.h"
#include "vkfunctions.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "internal_resources.h"
#include <iostream>
#include <vector>
#include "window.h"
#include "vkinstance.h"
#include "renderpass.h"
#include "command_buffer.h"

vkdev::UI::UI() {
  fpsCounter_ = 0;
  imguiPool_ = VK_NULL_HANDLE;
}

vkdev::UI::~UI() {

}

bool vkdev::UI::initUI(const Instance& i, const Window& w, 
                       const RenderPass& rp, const CommandBuffer& cmdbuffer, 
                       const uint32_t& images) {

  const uint32_t descriptor_number = 500;
  std::vector<VkDescriptorPoolSize> pool_sizes =
  {
    { VK_DESCRIPTOR_TYPE_SAMPLER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptor_number },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptor_number },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptor_number },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptor_number },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptor_number },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptor_number }
  };
  
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = descriptor_number;
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes = &pool_sizes[0];
  
  const VkDevice device = ResourceManager::GetResources()->device_;
  if (vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiPool_) != VK_NULL_HANDLE) {
      std::cout << "Failed to create UI descriptor pool" << std::endl;
      return false;
  }

  auto gqueue = ResourceManager::GetResources()->graphicsQueue_.get();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForVulkan(const_cast<GLFWwindow*>(w.getWindow()), true);
  ImGui_ImplVulkan_InitInfo init_info {};
  init_info.Instance = i.get();
  init_info.PhysicalDevice = ResourceManager::GetResources()->hardDevice_;
  init_info.Device = device;
  init_info.QueueFamily = gqueue->queueFamily_;
  init_info.Queue = gqueue->queue_;
  init_info.DescriptorPool = imguiPool_;
  init_info.MinImageCount = images;
  init_info.ImageCount = images;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  auto l = [](const char* function_name, void* user_data)
            { 
              using namespace vkdev;
              Instance* instance = reinterpret_cast<Instance*>(user_data);
              auto f = vkGetInstanceProcAddr(instance->get(), function_name);
              return f; 
            };
  vkdev::Instance* nconst_instance = const_cast<vkdev::Instance*>(&i);
  bool res = ImGui_ImplVulkan_LoadFunctions(l, reinterpret_cast<void*>(nconst_instance));
  ImGui_ImplVulkan_Init(&init_info, const_cast<VkRenderPass>(rp.getHandle()));

  const VkCommandBuffer cmd = cmdbuffer.getCommandBuffer();
  VkCommandBufferBeginInfo cmdBeginInfo{
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    nullptr,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    nullptr
  };

  vkBeginCommandBuffer(cmd, &cmdBeginInfo);

  ImGui_ImplVulkan_CreateFontsTexture(cmd);

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit_info {
    VK_STRUCTURE_TYPE_SUBMIT_INFO,
    nullptr,
    0,
    nullptr,
    nullptr,
    1,
    &cmd,
    0,
    nullptr
  };

  if (vkQueueSubmit(gqueue->queue_, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    std::cout << "Failed to submit UI info to GPU" << std::endl;
    return false;
  }

  vkQueueWaitIdle(gqueue->queue_);
  ImGui_ImplVulkan_DestroyFontUploadObjects();
  
  return true;
}

void vkdev::UI::updateFps(float fps) {
  fpsCounter_ = fps; 
}


void vkdev::UI::drawUI() const {
  int32_t width = 200;
  ImGui::SetNextWindowPos(ImVec2(WINDOW_W - width, 0));
  ImGui::SetNextWindowSize(ImVec2(width, 50));
  ImGui::Begin("Demo Info");
  ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / fpsCounter_), static_cast<int32_t>(fpsCounter_));
  ImGui::End();

  ImGui::Render();
}

void vkdev::UI::render() const {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  drawUI();
}

void vkdev::UI::destroyUI() {
  vkDestroyDescriptorPool(ResourceManager::GetResources()->device_, imguiPool_, nullptr);
  ImGui_ImplVulkan_Shutdown();
}