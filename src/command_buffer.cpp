#include "command_buffer.h"
#include <iostream>
#include "vkfunctions.h"
#include "commands/vkclear.h"
#include <functional>

vkdev::CommandBuffer::CommandBuffer(const int& buffer_size) {
    commandBuffer_.resize(buffer_size);
    commandList_.resize(buffer_size);
}

vkdev::CommandBuffer::~CommandBuffer() {
}

const VkCommandBuffer& vkdev::CommandBuffer::getCommandBuffer(uint32_t index) const {
    return commandBuffer_[index];
}

bool vkdev::CommandBuffer::submitCommand(const Command& command, const uint32_t buffer_order) {
    uint32_t t = command.getCommandType();
    if (t == CommandType_NONE) return false;

    std::shared_ptr<Command> new_cmd;
    switch(t) {
        case CommandType_Clear:
            new_cmd.reset(new ClearCmd(static_cast<const ClearCmd&>(command)));
        break;

        default:
            return false;
        break;
    }
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