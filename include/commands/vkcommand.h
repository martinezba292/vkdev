#ifndef __VK_COMMAND_H__
#define __VK_COMMAND_H__ 1

//#include <stdint.h>
#include "vulkan/vulkan.h"

enum CommandType {
  CommandType_NONE = -1,
  CommandType_Clear = 0,
  CommandType_BeginRenderPass = 1,
  CommandType_EndRenderPass,
  CommandType_BindPipeline,
  CommandType_BindVertexBuffer,
  CommandType_BindIndexBuffer,
  CommandType_BindDescriptor,
  CommandType_Draw,
  CommandType_DrawIndexed,
  CommandType_DrawUI,
  CommandType_MAX,
};

namespace vkdev {
  class CommandBuffer;
  class Command {
    public:
      Command(){}
      ~Command(){}
  
      virtual void record(const VkCommandBuffer& buffer) const = 0;
      uint32_t getCommandType() const {return type_;}
    
    protected:
     CommandType type_;

  };
}

#endif