#ifndef __COMMAND_BUFFERS_H__
#define __COMMAND_BUFFERS_H__ 1

#include "vulkan/vulkan.h"
#include <memory>
//#include "commands/vkcommand.h"
#include <list>

namespace vkdev {
  class Command;
  class Device;
  class CommandBuffer {
    public:
      CommandBuffer();
      ~CommandBuffer();
      bool allocateCommandBuffer(const Device& device, const VkCommandPool& pool, const uint32_t& count);
      bool allocateCommandBuffer(const Device& device, const uint32_t& count);
      void clearCommandList();
      bool destroyCommandBuffers();
      bool removeCommands(const uint32_t& first, const uint32_t& last);
      bool destroyCommandBuffersL(const VkCommandPool& pool);
      bool submitCommand(const Command& command);
      bool recordCommandBuffers(const VkCommandBufferUsageFlags& flags) const;
      const VkCommandBuffer& getCommandBuffer() const;

    private:
      const Device* deviceOwner_;
      VkCommandPool commandPool_;
      VkCommandBuffer commandBuffer_;
      std::list<std::unique_ptr<Command>> commandList_;
      //std::list<Command*> commandList_;
  };
}

#endif