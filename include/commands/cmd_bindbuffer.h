#ifndef __BIND_BUFFER_CMD_H__
#define __BIND_BUFFER_CMD_H__ 1

#include "vkcommand.h"
#include "vkfunctions.h"
//#include "vkbuffer.h"

namespace vkdev {
  class VlkInterface;
  struct BufferData;
  class BindBufferCmd : public Command {
    public:
      BindBufferCmd(uint64_t offset, uint32_t elements);
      ~BindBufferCmd() {}

      void record(const VkCommandBuffer& buffer) const override;
      // inline void record(const VkCommandBuffer& buffer) const override {
      //   VkDeviceSize offset = 0;
      //   vkCmdBindVertexBuffers(buffer, 0, 1, &handle_, &offset);
      // };

    private:
      const VkBuffer handle_;
      const uint64_t bindOffset_;
      const uint32_t count_;
      //const VkDeviceSize offset_;
  };
}

#endif