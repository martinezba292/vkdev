#include "pipeline_resources.h"
#include "vkhelpers.h"
#include "vkdevice.h"
#include "vkfunctions.h"
#include "renderpass.h"
#include <iostream>
#include <functional>

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

bool vkdev::PipelineResources::createGraphicsPipeline(const Device& device,
                                                      const RenderPass& rp,
                                                      const VertexParams& vertex_info,
                                                      const uint32_t& v_width,
                                                      const uint32_t& v_height,
                                                      const char* v_shader, 
                                                      const char* f_shader) {

  deviceOwner_ = &device.getDeviceHandle();

  VkShaderModule vertex_stage;
  if (!createShaderModule(device.getDeviceHandle(), v_shader, vertex_stage))
    return false;

  VkShaderModule fragment_stage;
  if (!createShaderModule(device.getDeviceHandle(), f_shader, fragment_stage))
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
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    VK_FALSE
   };

   //const float framebuffer_size = 300.0f;
  
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

  ShaderParams params{ParamsType_NONE};
  //VkDescriptorSetLayout* desc = params.layoutConfig.setLayout.data();
  if (!createPipelineLayout(device, params)) {
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
    nullptr,
    &color_blend_ci,
    nullptr,
    pipelineLayout_,
    rp.getHandle(),
    0,
    VK_NULL_HANDLE,
    -1
  };
  
  VkResult r = vkCreateGraphicsPipelines(*deviceOwner_, VK_NULL_HANDLE, 1, &graphics_pipe_ci, nullptr, &pipeline_);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't create graphics pipeline" << std::endl;
    return false;
  }

  vkDestroyShaderModule(*deviceOwner_, vertex_stage, nullptr);
  vkDestroyShaderModule(*deviceOwner_, fragment_stage, nullptr);

  return true;
}


bool vkdev::PipelineResources::createPipelineLayout(const Device& device, const ShaderParams& params) {
  if (pipelineLayout_ != VK_NULL_HANDLE) {
    std::cout << "Pipeline layout already created. Remove existing one in order to create another" << std::endl;    
    return false;
  }

  VkPipelineLayoutCreateInfo pipe_layout_ci {
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    nullptr,
    0
  };

  switch(params.type) {
    case ParamsType_DescriptorSets: {
      pipe_layout_ci.setLayoutCount = params.layoutConfig.setLayout.size();
      pipe_layout_ci.pSetLayouts = params.layoutConfig.setLayout.data();    
      break;
    }
    case ParamsType_PushConstants: {
      pipe_layout_ci.pushConstantRangeCount = params.layoutConfig.pushConstants.size();
      pipe_layout_ci.pPushConstantRanges = params.layoutConfig.pushConstants.data();
      break;
    }
    default:
    break;
  }
  
  VkResult r = vkCreatePipelineLayout(device.getDeviceHandle(), &pipe_layout_ci, nullptr, &pipelineLayout_);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't create pipeline layout" << std::endl;
    return false;
  }

  return true;
}

bool vkdev::PipelineResources::destroyPipelineResources() {
  if (pipelineLayout_ != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(*deviceOwner_, pipelineLayout_, nullptr);
    pipelineLayout_ = VK_NULL_HANDLE;
    if (pipeline_ != VK_NULL_HANDLE) {
      vkDestroyPipeline(*deviceOwner_, pipeline_, nullptr);
      pipeline_ = VK_NULL_HANDLE;
    }
    return true;
  }
  return false;
}