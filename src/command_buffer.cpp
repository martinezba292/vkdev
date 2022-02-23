#include "command_buffer.h"
#include <iostream>
#include "vkfunctions.h"
#include "commands/vkclear.h"
#include "commands/cmd_renderpass.h"
#include "commands/cmd_bindpipeline.h"
#include "commands/cmd_draw.h"
#include <functional>
#include "vkdevice.h"

vkdev::CommandBuffer::CommandBuffer(const int& buffer_size) {
    commandBuffer_.resize(buffer_size, VK_NULL_HANDLE);
    commandPool_ = VK_NULL_HANDLE;
    commandList_.resize(buffer_size);
}


vkdev::CommandBuffer::~CommandBuffer() {
    destroyCommandBuffers();
}

bool vkdev::CommandBuffer::destroyCommandBuffers() {

  if (commandList_.size() > 0) {
    for (auto& cmd_list : commandList_) {
      for (auto& cmd : cmd_list) {
        cmd.reset();
      }
      cmd_list.clear();
    }
    commandList_.clear();
  }
  
  if (commandBuffer_.size() > 0 && commandBuffer_[0] != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(deviceOwner_->getDeviceHandle());
    vkFreeCommandBuffers(deviceOwner_->getDeviceHandle(), commandPool_, commandBuffer_.size(), &commandBuffer_[0]);
    commandBuffer_.clear();
  }
  
  if (commandPool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(deviceOwner_->getDeviceHandle(), commandPool_, nullptr);
    commandPool_ = VK_NULL_HANDLE;
    return true;
  }
  
  return false;
}


const VkCommandBuffer& vkdev::CommandBuffer::getCommandBuffer(uint32_t index) const {
    return commandBuffer_[index];
}


bool vkdev::CommandBuffer::createCommandBuffer(const Device* device) {

    deviceOwner_ = device;

    VkCommandPoolCreateInfo pool_cinfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    const Device::DeviceQueue* queue = device->getDeviceQueue();
    pool_cinfo.flags = (VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | //Command buffer may be reset
                        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);            //Command buffer will be living for a shor time
    pool_cinfo.queueFamilyIndex = queue->queueFamily_;

    VkResult r = vkCreateCommandPool(device->getDeviceHandle(), &pool_cinfo, nullptr, &commandPool_);
    if (r != VK_SUCCESS) {
        std::cout << "Failed to create command pool" << std::endl;
        return false;
    }

    VkCommandBufferAllocateInfo buffer_alloc_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    buffer_alloc_info.commandPool = commandPool_;
    buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_alloc_info.commandBufferCount = commandBuffer_.size();

    r = vkAllocateCommandBuffers(device->getDeviceHandle(), &buffer_alloc_info, &commandBuffer_[0]);
    if (r != VK_SUCCESS) {
        std::cout << "An error ocurred during command buffer allocation" << std::endl;
        return false;
    }
    return true;
}


bool vkdev::CommandBuffer::submitCommand(const Command& command, const uint32_t buffer_order) {
    uint32_t t = command.getCommandType();
    if (t == CommandType_NONE) return false;

    //Must match CommandType enum order
    //See vkcommand.h 
    std::shared_ptr<Command> new_cmd;
    std::function<vkdev::Command*(void)>command_detect[] = {
      [&command](){ return new ClearCmd(static_cast<const ClearCmd&>(command));},
      [&command](){ return new RenderpassBeginCmd(static_cast<const RenderpassBeginCmd&>(command));},
      [&command](){ return new RenderpassEndCmd(static_cast<const RenderpassEndCmd&>(command));},
      [&command](){ return new BindPipelineCmd(static_cast<const BindPipelineCmd&>(command));},
      [&command](){ return new DrawCmd(static_cast<const DrawCmd&>(command));},
    };

    new_cmd.reset(command_detect[t]());
    commandList_[buffer_order].push_back(std::move(new_cmd));
    return true;
}


bool vkdev::CommandBuffer::recordCommandBuffers() const {
    if (commandBuffer_.size() < 1) 
        return false;

    if (commandList_.empty()) 
        return false;

    VkCommandBufferBeginInfo buffer_begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    for (size_t i = 0; i < commandBuffer_.size(); i++) {
      vkBeginCommandBuffer(commandBuffer_[i], &buffer_begin_info);
      for (auto& command : commandList_[i]) {
          command->record(commandBuffer_[i]);
      }

      if (vkEndCommandBuffer(commandBuffer_[i]) != VK_SUCCESS) {
          std::cout << "Error ocurred recording command buffers" << std::endl;
          return false;
      }
    }

    return true;  
}