#ifndef __BIND_INDEX_CMD_H__
#define __BIND_INDEX_CMD_H__ 1

#include "vkcommand.h"
#include "internal_resources.h"

namespace vkdev {
  class BindIndexBufferCmd : public Command {
    public:
      BindIndexBufferCmd(){
        type_ = CommandType_BindIndexBuffer;
      }
      
      ~BindIndexBufferCmd(){}

      inline void record(const VkCommandBuffer& buffer) const override {
        const VkBuffer& handle = ResourceManager::GetResources()->indexBufferMemory_->getHandle();
        vkCmdBindIndexBuffer(buffer, handle, 0, VK_INDEX_TYPE_UINT32);
      }
  };
}


#endif