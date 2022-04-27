#ifndef __CMD_BUFFERCOPY__
#define __CMD_BUFFERCOPY__ 1

#include "vkcommand.h"

namespace vkdev {
  class BufferCopyCmd : public Command {
    public:
      BufferCopyCmd();
      ~BufferCopyCmd();

      inline void record(const VkCommandBuffer& buffer) const override {
          vkCmdCopyBuffer(buffer, )
      }
  };
}

#endif