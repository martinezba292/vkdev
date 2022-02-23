#ifndef __VK_HELPERS__
#define __VK_HELPERS__ 1

#include "common_def.h"
#include <vector>
#include <string>

enum VertexParams : uint8_t;
struct VertexInputDescriptors;
extern "C" {
  namespace vkdev {
      bool checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& available_extensions);
      bool createImageView(const VkDevice& device, const ImageFormat& format, const ImageHandle& img, VkImageView& result);
      bool createShaderModule(const VkDevice& device, const char* path, VkShaderModule& module);
      
      void setInputDescriptors(const uint32_t& binding, 
                               const VertexParams& vparams,
                               VertexInputDescriptors& descriptors);
                               
      void setBindingDescription(const uint32_t& binding, 
                                 const uint32_t& stride, 
                                 const VkVertexInputRate& input_rate, 
                                 VkVertexInputBindingDescription& descriptor);

     void setAttributeDescription(const uint32_t& location, 
                                  const uint32_t& binding, 
                                  const VkFormat& format,
                                  const uint32_t& offset,
                                  VkVertexInputAttributeDescription& descriptor);
  }
}

#endif