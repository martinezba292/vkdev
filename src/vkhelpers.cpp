#include "vkhelpers.h"
#include "vkfunctions.h"
#include "pipeline_resources.h"
#include "glm.hpp"
#include <iostream>
#include <string.h>
#include <fstream>

bool vkdev::checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& available_extensions) {
  for (auto ext : available_extensions) {
  if (strcmp(extension, ext.extensionName) == 0)
      return true;
  }

  return false;
}

bool vkdev::createImageView(const VkDevice& device, const ImageFormat& format, const ImageHandle& img, VkImageView& result) {

  VkImageSubresourceRange subres_range;
  subres_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subres_range.baseArrayLayer = 0;
  subres_range.baseMipLevel = 0;
  subres_range.levelCount = 1;
  subres_range.layerCount = 1;

  VkImageViewCreateInfo image_view_ci {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  image_view_ci.image = img;
  image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_ci.format = format;
  image_view_ci.components = {
                               VK_COMPONENT_SWIZZLE_IDENTITY,
                               VK_COMPONENT_SWIZZLE_IDENTITY,
                               VK_COMPONENT_SWIZZLE_IDENTITY,
                               VK_COMPONENT_SWIZZLE_IDENTITY,
                             };
  image_view_ci.subresourceRange = subres_range;

  VkResult r = vkCreateImageView(device, &image_view_ci, nullptr, &result);
  if (r != VK_SUCCESS) {
      std::cout << "Failed to create image view" << std::endl;
      return false;
  }
  
  return true;
}


bool vkdev::createShaderModule(const VkDevice& device, const char* path, VkShaderModule& module) {
  
  std::ifstream shader_file(path, std::ios::ate | std::ios::binary);
  if (!shader_file.is_open()) {
    std::cout << "Couldn't open shader file from path" << std::endl;
    return false;
  }

  size_t filesize = shader_file.tellg();
  std::vector<char> code(filesize);
  shader_file.seekg(0);
  shader_file.read(code.data(), filesize);
  shader_file.close();

  VkShaderModuleCreateInfo shader_module_ci {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  shader_module_ci.codeSize = code.size();
  shader_module_ci.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkResult r = vkCreateShaderModule(device, &shader_module_ci, nullptr, &module);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create shader module" << std::endl;
    return false;
  }

  return true;
} 

void vkdev::setInputDescriptors(const uint32_t& binding, 
                                const VertexParams& vparams,
                                VertexInputDescriptors& descriptors) {
  uint32_t stride = vparams * sizeof(glm::vec3);
  if (vparams > VertexParams_3V_3N) 
    stride -= sizeof(float);

  descriptors.bindDescription = {binding, stride, VK_VERTEX_INPUT_RATE_VERTEX};
  VkFormat formats[] = {
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32_SFLOAT,
  };
  
  descriptors.attribDescription.reserve(vparams);
  for (uint32_t i = 0; i < vparams; i++) {
    descriptors.attribDescription[i] = {
      i,
      binding,
      formats[i],
      static_cast<uint32_t>(sizeof(glm::vec3) * i)
    };
  }
}


void vkdev::setBindingDescription(const uint32_t& binding, 
                                  const uint32_t& stride, 
                                  const VkVertexInputRate& input_rate, 
                                  VkVertexInputBindingDescription& descriptor) {
  descriptor.binding = binding;
  descriptor.stride = stride;
  descriptor.inputRate = input_rate;
}


void vkdev::setAttributeDescription(const uint32_t& location, 
                                    const uint32_t& binding, 
                                    const VkFormat& format,
                                    const uint32_t& offset,
                                    VkVertexInputAttributeDescription& descriptor) {
  descriptor.location = location;
  descriptor.binding = binding;
  descriptor.format = format;
  descriptor.offset = offset;
}