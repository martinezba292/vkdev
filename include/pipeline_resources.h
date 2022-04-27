#ifndef __PIPELINE_RESOURCES_H__
#define __PIPELINE_RESOURCES_H__ 1

#include "common_def.h"
#include <vector>

enum VertexParams : uint8_t {
  VertexParams_NONE = 0,
  VertexParams_3V = 1,
  VertexParams_3V_3N = 2,
  VertexParams_3V_3N_2U = 3,
};

struct VertexInputDescriptors {
  VkVertexInputBindingDescription bindDescription;
  std::vector<VkVertexInputAttributeDescription> attribDescription;
};

// enum ParamsType {
//   ParamsType_NONE = 0,
//   ParamsType_DescriptorSets = 1,
//   ParamsType_PushConstants = 2
// };

// struct ShaderParams {
//   ParamsType type;
//   union Params {
//     Params(){}
//     ~Params(){}
//     std::vector<VkDescriptorSetLayout> setLayout;
//     std::vector<VkPushConstantRange> pushConstants;
//   }layoutConfig;
// };

namespace vkdev {
  class RenderPass;
  class PipelineDescriptor;
  class PipelineResources {
    public:
      PipelineResources();
      ~PipelineResources();
      bool createGraphicsPipeline(const PipelineDescriptor& descriptor,
                                  const RenderPass& rp, 
                                  const VertexParams& vertex_info,
                                  const uint32_t& v_width,
                                  const uint32_t& v_height, 
                                  const char* v_shader, 
                                  const char* f_shader);

      const VkPipeline& getHandle()const;
      const VkPipelineLayout& getLayout() const;
      bool destroyPipelineResources();
      //bool createPipelineLayout(const Device& device, const ShaderParams& params);

    private:
      VkPipeline pipeline_;
      VkPipelineLayout pipelineLayout_;
  };
}


#endif