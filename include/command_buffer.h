#ifndef __COMMAND_BUFFERS_H__
#define __COMMAND_BUFFERS_H__ 1

#include "vulkan/vulkan.h"
#include <map>
#include <memory>
#include <list>
#include <vector>

namespace vkdev {
  class Command;
  class CommandBuffer {
    public:
      CommandBuffer(const int& buffer_size);
      ~CommandBuffer();
      const VkCommandBuffer& getCommandBuffer(uint32_t index) const;

      bool submitCommand(const Command& command, const uint32_t buffer_order);
      bool recordCommandBuffers() const;

    private:
      VkCommandPool commandPool_;
      std::vector<VkCommandBuffer> commandBuffer_;
      std::vector<std::list<std::shared_ptr<Command>>> commandList_;
      //std::multimap<const uint32_t, const Command*> commands_;

  };
}

#endif