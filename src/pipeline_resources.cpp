#include "pipeline_resources.h"
#include "vkhelpers.h"
#include "vkfunctions.h"
#include "renderpass.h"
#include <iostream>
#include "internal_resources.h"
#include "pipeline_descriptor.h"

vkdev::PipelineResources::PipelineResources() {
  pipeline_ = VK_NULL_HANDLE;
  pipelineLayout_ = VK_NULL_HANDLE;
}

vkdev::PipelineResources::~PipelineResources() {
  destroyPipelineResources();
}

const VkPipeline& vkdev::PipelineResources::getHandle() const{
  return pipeline_;
}

const VkPipelineLayout& vkdev::PipelineResources::getLayout() const {
  return pipelineLayout_;
}

bool vkdev::PipelineResources::createGraphicsPipeline(const PipelineDescriptor& descriptor,
                                                      const RenderPass& rp,
                                                      const VertexParams& vertex_info,
                                                      const uint32_t& v_width,
                                                      const uint32_t& v_height,
                                                      const char* v_shader, 
                                                      const char* f_shader) {

  const VkDevice& deviceh = ResourceManager::GetResources()->device_;

  VkShaderModule vertex_stage;
  if (!createShaderModule(deviceh, v_shader, vertex_stage))
    return false;

  VkShaderModule fragment_stage;
  if (!createShaderModule(deviceh, f_shader, fragment_stage))
    return false;

  std::vector<VkPipelineShaderStageCreateInfo> shader_stage_ci = {
    {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      nullptr,
      0,
      VK_SHADER_STAGE_VERTEX_BIT,
      vertex_stage,
      "main",
      nullptr
    },
    {
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      nullptr,
      0,
      VK_SHADER_STAGE_FRAGMENT_BIT,
      fragment_stage,
      "main",
      nullptr
    }    
  };

  VertexInputDescriptors input_desc;
  setInputDescriptors(0, vertex_info, input_desc);
  VkPipelineVertexInputStateCreateInfo vertex_input_ci 
  {
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    nullptr,
    0,
    1,
    &input_desc.bindDescription,
    static_cast<uint32_t>(input_desc.attribDescription.size()),
    &input_desc.attribDescription[0]
  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly_ci {
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    nullptr,
    0,
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VK_FALSE
   };

  
  //Not using Dynamic state so viewport and scissor can be declared static
   VkViewport viewport {
     0.0f,
     0.0f,
     static_cast<float>(v_width),
     static_cast<float>(v_height),
     0.0f,
     1.0f,
   };

   VkRect2D scissor {
     {
       0,
       0
     },
     {
       v_width,
       v_height
     }
   };

    VkPipelineViewportStateCreateInfo pipeline_viewport_ci {
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      nullptr,
      0,
      1,  //viewport count
      &viewport,
      1,  //scissor count
      &scissor
    };

    VkPipelineRasterizationStateCreateInfo pipe_rasterization_ci {
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      nullptr,
      0,
      VK_FALSE,
      VK_FALSE,
      VK_POLYGON_MODE_FILL,
      VK_CULL_MODE_BACK_BIT,
      VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE,
      0.0f,
      0.0f,
      0.0f,
      1.0f,
    };

    VkPipelineMultisampleStateCreateInfo pipe_multisample_ci {
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      nullptr,
      0,
      VK_SAMPLE_COUNT_1_BIT,
      VK_FALSE,
      1.0f,
      nullptr,
      VK_FALSE,
      VK_FALSE
    };

    VkPipelineColorBlendAttachmentState color_blend_attach {
      VK_FALSE,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_BLEND_FACTOR_ONE,
      VK_BLEND_FACTOR_ZERO,
      VK_BLEND_OP_ADD,
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

  VkPipelineColorBlendStateCreateInfo color_blend_ci {
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    nullptr,
    0,
    VK_FALSE,
    VK_LOGIC_OP_COPY,
    1,
    &color_blend_attach,
    {0.0f, 0.0f, 0.0f, 0.0f}
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil_ci {
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // VkStructureType                            sType;
    nullptr,                                                    // const void*                                pNext;                                                          
    0,                                                          // VkPipelineDepthStencilStateCreateFlags flags;
    VK_TRUE,                                                    // VkBool32                               depthTestEnable;      
    VK_TRUE,                                                    // VkBool32                               depthWriteEnable;      
    VK_COMPARE_OP_LESS,                                         // VkCompareOp                            depthCompareOp;                
    VK_FALSE,                                                   // VkBool32                               depthBoundsTestEnable;      
    VK_FALSE,                                                   // VkBool32                               stencilTestEnable;      
    {},                                                         // VkStencilOpState                       front;
    {},                                                         // VkStencilOpState                       back;
    0.0f,                                                       // float                                  minDepthBounds;  
    1.0f                                                        // float                                  maxDepthBounds;  
  };

  std::vector<VkDescriptorSetLayout> layouts;
  descriptor.getDescriptorLayout(layouts);

  VkPipelineLayoutCreateInfo pipe_layout_ci {
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    nullptr,
    0,
    static_cast<uint32_t>(layouts.size()),
    &layouts[0]
  };

  VkResult r = vkCreatePipelineLayout(deviceh, &pipe_layout_ci, nullptr, &pipelineLayout_);
  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred during pipeline layout creation" << std::endl;
    return false;
  }

  VkGraphicsPipelineCreateInfo graphics_pipe_ci {
    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    nullptr,
    0,
    static_cast<uint32_t>(shader_stage_ci.size()),
    &shader_stage_ci[0],
    &vertex_input_ci,
    &input_assembly_ci,
    nullptr,
    &pipeline_viewport_ci,
    &pipe_rasterization_ci,
    &pipe_multisample_ci,
    &depth_stencil_ci,
    &color_blend_ci,
    nullptr,
    pipelineLayout_,
    rp.getHandle(),
    0,
    VK_NULL_HANDLE,
    -1
  };
  
  r = vkCreateGraphicsPipelines(deviceh, VK_NULL_HANDLE, 1, &graphics_pipe_ci, nullptr, &pipeline_);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't create graphics pipeline" << std::endl;
    destroyPipelineResources();
    return false;
  }

  vkDestroyShaderModule(deviceh, vertex_stage, nullptr);
  vkDestroyShaderModule(deviceh, fragment_stage, nullptr);

  return true;
}


bool vkdev::PipelineResources::destroyPipelineResources() {
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  if (pipelineLayout_ != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(deviceh, pipelineLayout_, nullptr);
    pipelineLayout_ = VK_NULL_HANDLE;
    if (pipeline_ != VK_NULL_HANDLE) {
      vkDestroyPipeline(deviceh, pipeline_, nullptr);
      pipeline_ = VK_NULL_HANDLE;
    }
    return true;
  }
  return false;
}