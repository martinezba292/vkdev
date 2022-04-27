#ifndef __CMD_BIND_DESCRIPTOR_H__
#define __CMD_BIND_DESCRIPTOR_H__ 1

#include "vkcommand.h"

namespace vkdev {
  class PipelineDescriptor;
  class PipelineResources;
  class BindDescriptorCmd : public Command {
    public:
      BindDescriptorCmd(const PipelineDescriptor* descriptor, const PipelineResources* layout, 
                        const uint32_t& first, const uint32_t& dynamic_offset = 0) : 
      descSet_(descriptor), firstSet_(first), pipeline_(layout), offset_(dynamic_offset)
       { type_ = CommandType_BindDescriptor; }
      
      ~BindDescriptorCmd() {
        descSet_ = nullptr;
        pipeline_ = nullptr;
      }

      void addDynamicOffset(const uint32_t& offset){ offset_ += offset; }

      void record(const VkCommandBuffer& cmd) const override;

    private:
      const PipelineDescriptor* descSet_;
      const PipelineResources* pipeline_;
      const uint32_t firstSet_;
      uint32_t offset_;
  };
}

#endif