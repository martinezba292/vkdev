#include "command_buffer.h"
#include <iostream>
#include "vkfunctions.h"
#include "commands/vkclear.h"
#include "commands/cmd_renderpass.h"
#include "commands/cmd_bindpipeline.h"
#include "commands/cmd_draw.h"
#include "commands/cmd_bindbuffer.h"
#include "commands/cmd_binddescriptor.h"
#include "commands/cmd_bindindex.h"
#include "commands/cmd_drawindexed.h"
#include "commands/cmd_drawUI.h"
#include <functional>
#include "vkdevice.h"
#include "internal_resources.h"
#include "vkhelpers.h"

vkdev::CommandBuffer::CommandBuffer() {
    commandBuffer_ = VK_NULL_HANDLE;
    commandPool_ = VK_NULL_HANDLE;
    commandList_.clear();
}


vkdev::CommandBuffer::~CommandBuffer() {
    destroyCommandBuffers();
}

bool vkdev::CommandBuffer::destroyCommandBuffers() {

  if (commandList_.size() > 0) {
    commandList_.clear();
  }
  
  if (commandBuffer_ != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(deviceOwner_->getDeviceHandle());
    vkFreeCommandBuffers(deviceOwner_->getDeviceHandle(), commandPool_, 1, &commandBuffer_);
    commandBuffer_ = VK_NULL_HANDLE;
  }
  
  if (commandPool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(deviceOwner_->getDeviceHandle(), commandPool_, nullptr);
    commandPool_ = VK_NULL_HANDLE;
    return true;
  }
  
  return false;
}


bool vkdev::CommandBuffer::destroyCommandBuffersL(const VkCommandPool& pool) {
  if (commandList_.size() > 0) {
    commandList_.clear();
  }

  if (commandBuffer_ == VK_NULL_HANDLE)
    return false;

  vkDeviceWaitIdle(deviceOwner_->getDeviceHandle());
  vkFreeCommandBuffers(deviceOwner_->getDeviceHandle(), pool, 1, &commandBuffer_);
  commandBuffer_ = VK_NULL_HANDLE;

  return true;
}


const VkCommandBuffer& vkdev::CommandBuffer::getCommandBuffer() const {
  return commandBuffer_;
}



bool vkdev::CommandBuffer::allocateCommandBuffer(const Device& device, const VkCommandPool& pool, const uint32_t& count) {
  deviceOwner_ = &device;

  VkCommandBufferAllocateInfo buffer_alloc_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  buffer_alloc_info.commandPool = pool;
  buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  buffer_alloc_info.commandBufferCount = count;

  VkResult r = vkAllocateCommandBuffers(device.getDeviceHandle(), &buffer_alloc_info, &commandBuffer_);
  if (r != VK_SUCCESS) {
      std::cout << "An error ocurred during command buffer allocation" << std::endl;
      return false;
  }

  return true;
}


bool vkdev::CommandBuffer::allocateCommandBuffer(const Device& device, const uint32_t& count) {
  deviceOwner_ = &device;

  const auto* queue = ResourceManager::GetResources()->graphicsQueue_.get();
  createCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | //Command buffer may be reset
                    VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,             //Command buffer will be living for a short time
                    queue->queueFamily_, commandPool_);

  VkCommandBufferAllocateInfo buffer_alloc_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  buffer_alloc_info.commandPool = commandPool_;
  buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  buffer_alloc_info.commandBufferCount = count;

  VkResult r = vkAllocateCommandBuffers(device.getDeviceHandle(), &buffer_alloc_info, &commandBuffer_);
  if (r != VK_SUCCESS) {
      std::cout << "An error ocurred during command buffer allocation" << std::endl;
      return false;
  }

  return true;
}

void vkdev::CommandBuffer::clearCommandList() {
  commandList_.clear();
}

bool vkdev::CommandBuffer::removeCommands(const uint32_t& first, const uint32_t& last) {
  size_t s = commandList_.size();
  if (s < first || s < last) 
    return false;

  auto it = commandList_.begin();
  auto it2 = commandList_.begin();
  std::advance(it, first);
  std::advance(it2, last);
  commandList_.erase(it, it2);

  return true;
}

bool vkdev::CommandBuffer::submitCommand(const Command& command) {
  uint32_t t = command.getCommandType();
  if (t == CommandType_NONE) return false;

  //Must match CommandType enum order
  //See vkcommand.h 
  std::function<std::unique_ptr<vkdev::Command>(void)>command_detect[] = {
    [&command](){ return std::make_unique<ClearCmd>(static_cast<const ClearCmd&>(command));},
    [&command](){ return std::make_unique<RenderpassBeginCmd>(static_cast<const RenderpassBeginCmd&>(command));},
    [&command](){ return std::make_unique<RenderpassEndCmd>(static_cast<const RenderpassEndCmd&>(command));},
    [&command](){ return std::make_unique<BindPipelineCmd>(static_cast<const BindPipelineCmd&>(command));},
    [&command](){ return std::make_unique<BindBufferCmd>(static_cast<const BindBufferCmd&>(command));},
    [&command](){ return std::make_unique<BindIndexBufferCmd>(static_cast<const BindIndexBufferCmd&>(command));},
    [&command](){ return std::make_unique<BindDescriptorCmd>(static_cast<const BindDescriptorCmd&>(command));},
    [&command](){ return std::make_unique<DrawCmd>(static_cast<const DrawCmd&>(command));},
    [&command](){ return std::make_unique<DrawIndexedCmd>(static_cast<const DrawIndexedCmd&>(command));},
    [&command](){ return std::make_unique<DrawUICmd>(static_cast<const DrawUICmd&>(command));}
  };

  commandList_.push_back(std::move(command_detect[t]()));
  //commandList_.push_back(std::move(&command));

  return true;
}


bool vkdev::CommandBuffer::recordCommandBuffers(const VkCommandBufferUsageFlags& flags) const {
  if (commandBuffer_ == VK_NULL_HANDLE) 
      return false;

  if (commandList_.empty()) 
      return false;

  VkCommandBufferBeginInfo buffer_begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  buffer_begin_info.flags = flags;

  vkBeginCommandBuffer(commandBuffer_, &buffer_begin_info);
  for (auto& command : commandList_) {
      command->record(commandBuffer_);
  }

  if (vkEndCommandBuffer(commandBuffer_) != VK_SUCCESS) {
      std::cout << "Error ocurred recording command buffers" << std::endl;
      return false;
  }

  return true;  
}