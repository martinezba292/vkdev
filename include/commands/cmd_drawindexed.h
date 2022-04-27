#ifndef __DRAW_INDEXED_CMD_H__
#define __DRAW_INDEXED_CMD_H__ 1

#include "vkcommand.h"
#include "internal_resources.h"

namespace vkdev {
  class DrawIndexedCmd : public Command {
    public:
      DrawIndexedCmd(const uint64_t& first_vertex, 
                     const uint64_t& first_index,
                     const uint32_t& count) : vOffset_(first_vertex), 
                                              iOffset_(first_index), 
                                              elements(count) {
        type_ = CommandType_DrawIndexed;
      }

      ~DrawIndexedCmd(){}

      inline void record(const VkCommandBuffer& buffer) const override {
        vkCmdDrawIndexed(buffer, elements, 1, iOffset_, vOffset_, 0);
      }

    private:
      const uint64_t vOffset_;
      const uint64_t iOffset_;
      const uint32_t elements;

  };
}


#endif