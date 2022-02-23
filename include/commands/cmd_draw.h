#ifndef __CMD_DRAW_H__
#define __CMD_DRAW_H__ 1

#include "vkcommand.h"
#include "vkfunctions.h"

namespace vkdev {
  class DrawCmd : public Command {
    public:
      inline DrawCmd(const uint32_t vertex_number, const uint32_t instances) : 
      n_vertex(vertex_number), n_instances(instances){type_ = CommandType_Draw;};
      ~DrawCmd(){};
      
      inline void record(const VkCommandBuffer& buffer) const override {
        vkCmdDraw(buffer, n_vertex, n_instances, 0, 0);
      }

    private:
      uint32_t n_vertex;
      uint32_t n_instances;
  };
}



#endif
