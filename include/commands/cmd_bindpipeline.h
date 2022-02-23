#ifndef __CMD_BINDPIPELINE_H__
#define __CMD_BINDPIPELINE_H__ 1

#include "vkcommand.h"
#include "vkfunctions.h"
#include "pipeline_resources.h"

namespace vkdev {
  //class PipelineResources;
  class BindPipelineCmd : public Command {
    public:
      BindPipelineCmd(const PipelineResources& pipeline) : 
      pipeline_(&pipeline.getHandle()){type_ = CommandType_BindPipeline;};
      ~BindPipelineCmd(){pipeline_ = nullptr;};
      inline void record(const VkCommandBuffer& buffer) const override {
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline_);
      };

    private:
    const VkPipeline* pipeline_;
  };
}


#endif