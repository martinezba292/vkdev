#include "vkhelpers.h"
#include "vkfunctions.h"
#include "pipeline_resources.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include "internal_resources.h"
#include "glfw3.h"

bool vkdev::checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& available_extensions) {
  for (auto ext : available_extensions) {
  if (strcmp(extension, ext.extensionName) == 0)
      return true;
  }

  return false;
}

bool vkdev::createImage(ImageHandle& image, const uint32_t& width, const uint32_t& height, 
                        const VkImageType& type, const VkFormat& format, const VkImageUsageFlags& usage) {

  VkImageCreateInfo image_ci {
    VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    nullptr,
    0,
    type,
    format,
    {
      width,
      height,
      1,
    },
    1,
    1,
    VK_SAMPLE_COUNT_1_BIT,
    VK_IMAGE_TILING_OPTIMAL,
    usage,
    VK_SHARING_MODE_EXCLUSIVE,
    0,
    nullptr,
    VK_IMAGE_LAYOUT_UNDEFINED
  };

  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  VkResult r = vkCreateImage(hdevice, &image_ci, nullptr, &image);

  if (r != VK_SUCCESS) {
    std::cout << "Failed to create image handle" << std::endl;
    return false;
  }

  return true;
}

bool vkdev::allocateImageMemory(VkDeviceMemory& memory, const ImageHandle& image, const VkMemoryPropertyFlagBits& properties) {

  const VkDevice& hdevice = ResourceManager::GetResources()->device_;

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(ResourceManager::GetResources()->hardDevice_, &memory_properties);

  VkMemoryRequirements image_memory_requirements;
  vkGetImageMemoryRequirements(hdevice, image, &image_memory_requirements);

  for (size_t i = 0; i < memory_properties.memoryTypeCount; i++) {
  //Check for minimum buffer memory requirements against physical device properties
    if ( (image_memory_requirements.memoryTypeBits & (1 << i)) && 
        (memory_properties.memoryTypes[i].propertyFlags & properties)) {
        
      VkMemoryAllocateInfo allocate_info {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        image_memory_requirements.size,
        static_cast<uint32_t>(i)
      };

      VkResult r = vkAllocateMemory(hdevice, &allocate_info, nullptr, &memory);
      if (r != VK_SUCCESS) {
        std::cout << "Couldn't allocate buffer memory" << std::endl;
        return false;
      }
    }
  }

  return true;
}


bool vkdev::createImageView(const VkDevice& device, const ImageFormat& format, const ImageHandle& img, 
                            VkImageView& result, const VkImageAspectFlags& aspect/*= VK_IMAGE_ASPECT_COLOR_BIT*/) {

  VkImageSubresourceRange subres_range {};
  subres_range.aspectMask = aspect;
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

bool vkdev::createSampler(VkSampler& sample, const VkFilter& filter) {
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  VkSamplerCreateInfo sampler_ci {
    VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,  // VkStructureType        sType
    nullptr,                                // const void*            pNext
    0,                                      // VkSamplerCreateFlags   flags
    filter,                                 // VkFilter               magFilter
    filter,                                     // VkFilter               minFilter
    VK_SAMPLER_MIPMAP_MODE_NEAREST,         // VkSamplerMipmapMode    mipmapMode
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeU
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeV
    VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // VkSamplerAddressMode   addressModeW
    0.0f,                                   // float                  mipLodBias
    VK_FALSE,                               // VkBool32               anisotropyEnable
    1.0f,                                   // float                  maxAnisotropy
    VK_FALSE,                               // VkBool32               compareEnable
    VK_COMPARE_OP_ALWAYS,                   // VkCompareOp            compareOp
    0.0f,                                   // float                  minLod
    0.0f,                                   // float                  maxLod
    VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,// VkBorderColor          borderColor
    VK_FALSE                                // VkBool32               unnormalizedCoordinates
  };

  VkResult r = vkCreateSampler(deviceh, &sampler_ci, nullptr, &sample);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create image sampler" << std::endl;
    return false;
  }

  return true;
}


bool vkdev::createCommandPool(const VkCommandPoolCreateFlags& flags, 
                       const uint32_t& queue_family, VkCommandPool& pool) {

  auto resources = vkdev::ResourceManager::GetResources();
  VkCommandPoolCreateInfo pool_cinfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  const auto* queue = resources->graphicsQueue_.get();
  pool_cinfo.flags = flags;
  pool_cinfo.queueFamilyIndex = queue->queueFamily_;

  VkResult r = vkCreateCommandPool(resources->device_, &pool_cinfo, nullptr, &pool);
  if (r != VK_SUCCESS) {
      std::cout << "Failed to create command pool" << std::endl;
      return false;
  }

  return true;
}

bool vkdev::destroyCommandPool(VkCommandPool& pool) {
  if (pool != VK_NULL_HANDLE) {
    auto resources = vkdev::ResourceManager::GetResources();
    vkDestroyCommandPool(resources->device_, pool, nullptr);
    pool = VK_NULL_HANDLE;
    return true;
  }
  return false;
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
  
  uint32_t dim = (sizeof(float) * 3);
  uint32_t stride = (vparams * dim);
  if (vparams > VertexParams_3V_3N) 
    stride -= sizeof(float);

  descriptors.bindDescription = {binding, stride, VK_VERTEX_INPUT_RATE_VERTEX};
  VkFormat formats[] = {
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32_SFLOAT,
  };
  
  descriptors.attribDescription.resize(vparams);
  for (uint32_t i = 0; i < vparams; i++) {
    descriptors.attribDescription[i] = {
      i,
      binding,
      formats[i],
      static_cast<uint32_t>(dim * i)
    };
  }
}


void* vkdev::alignedAlloc(size_t size, size_t alignment) {
  void* data = nullptr;

  #if defined (_MSC_VER) || (__MINGW32__)
    data = _aligned_malloc(size, alignment);
  #else
    int32_t res = posix_memalign(&data, alignment, size);
    if(res != 0)
      data = nullptr;
  #endif

  return data;
}


void vkdev::alignedFree(void* data) {
  #if defined (_MSC_VER) || (__MINGW32__)
    _aligned_free(data);
  #else
    free(data);
  #endif
}

std::vector<const char*> vkdev::getRequiredExtensions(bool enableValidationLayers) {
  uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkdev::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkFormat vkdev::findSupportedFormats(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features){
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(ResourceManager::GetResources()->hardDevice_, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("Format candidates not supported by device");
}


void vkdev::destroyTexture(TextureData& texture) {
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  if (texture.img_ != VK_NULL_HANDLE) {
    vkDestroyImage(deviceh, texture.img_, nullptr);
    texture.img_ = VK_NULL_HANDLE;
  }

  if (texture.imgView_ != VK_NULL_HANDLE) {
    vkDestroyImageView(deviceh, texture.imgView_, nullptr);
    texture.imgView_ = VK_NULL_HANDLE;
  } 

  if (texture.sampler_ != VK_NULL_HANDLE) {
    vkDestroySampler(deviceh, texture.sampler_, nullptr);
    texture.sampler_ = VK_NULL_HANDLE;
  } 

  if (texture.imgMemory_ != VK_NULL_HANDLE) {
    vkFreeMemory(deviceh, texture.imgMemory_, nullptr);
    texture.imgMemory_ = VK_NULL_HANDLE;
  }
}