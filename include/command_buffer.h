#ifndef __COMMAND_BUFFERS_H__
#define __COMMAND_BUFFERS_H__ 1

#include "vulkan/vulkan.h"
#include <map>
#include <memory>
#include <list>
#include <vector>

namespace vkdev {
  class Command;
  class Device;
  class CommandBuffer {
    public:
      CommandBuffer(const int& buffer_size);
      ~CommandBuffer();
      bool createCommandBuffer(const Device* device);
      bool destroyCommandBuffers();
      bool submitCommand(const Command& command, const uint32_t buffer_order);
      bool recordCommandBuffers() const;
      const VkCommandBuffer& getCommandBuffer(uint32_t index) const;

    private:
      const Device* deviceOwner_;
      VkCommandPool commandPool_;
      std::vector<VkCommandBuffer> commandBuffer_;
      std::vector<std::list<std::shared_ptr<Command>>> commandList_;

  };
}

#endif