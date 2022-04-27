#ifndef __VK_HELPERS__
#define __VK_HELPERS__ 1

#include "common_def.h"
#include <vector>
#include <string>

enum VertexParams : uint8_t;
struct VertexInputDescriptors;
extern "C" {
  namespace vkdev {
  struct TextureData;

    bool checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& available_extensions);

    VkFormat findSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    bool createImage(ImageHandle& image, const uint32_t& width, const uint32_t& height, 
                     const VkImageType& type, const VkFormat& format, const VkImageUsageFlags& usage);

    bool allocateImageMemory(VkDeviceMemory& memory, const ImageHandle& image, const VkMemoryPropertyFlagBits& properties);

    bool createImageView(const VkDevice& device, const ImageFormat& format, const ImageHandle& img, 
                         VkImageView& result, const VkImageAspectFlags& aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    bool createSampler(VkSampler& sample, const VkFilter& filter);

    void destroyTexture(TextureData& texture);


    bool createShaderModule(const VkDevice& device, const char* path, VkShaderModule& module);
  
    //Manage command pool independently of command buffers
    bool createCommandPool(const VkCommandPoolCreateFlags& flags, 
                            const uint32_t& queue_family, VkCommandPool& pool);
  
    bool destroyCommandPool(VkCommandPool& pool);
                            
  
    void setInputDescriptors(const uint32_t& binding, 
                              const VertexParams& vparams,
                              VertexInputDescriptors& descriptors);


    void* alignedAlloc(size_t size, size_t alignment);
    
    void alignedFree(void* data);
                              
    std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                 VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                 void* pUserData);

    // bool createBuffer(const VkBufferUsageFlags& usage, 
    //                   const VkMemoryPropertyFlagBits& properties, 
    //                   VkBuffer& buffer, VkDeviceMemory& buffer_mem,
    //                   const VkDeviceSize& size);

    // bool mapBufferMemory(const void* data, const VkDeviceMemory& memory, const VkDeviceSize& size);

    // bool destroyBuffer(VkBuffer& buffer, VkDeviceMemory& memory);


    // void setBindingDescription(const uint32_t& binding, 
    //                             const uint32_t& stride, 
    //                             const VkVertexInputRate& input_rate, 
    //                             VkVertexInputBindingDescription& descriptor);
  
    // void setAttributeDescription(const uint32_t& location, 
    //                               const uint32_t& binding, 
    //                               const VkFormat& format,
    //                               const uint32_t& offset,
    //                               VkVertexInputAttributeDescription& descriptor);
        
  }
}

#endif