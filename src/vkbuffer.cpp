#include "vkbuffer.h"
#include "internal_resources.h"
#include "vkfunctions.h"
#include <iostream>
#include <string.h>


vkdev::Buffer::Buffer() {
  bufferHandle_ = VK_NULL_HANDLE;
  bufferMemory_ = VK_NULL_HANDLE;
  bufferSize_ = 0;
}

 /****************************************************************************/

vkdev::Buffer::Buffer(const Buffer& buffer) {
  bufferHandle_ = buffer.bufferHandle_;
  bufferMemory_ = buffer.bufferMemory_;
  bufferSize_ = buffer.bufferSize_;
}

/****************************************************************************/

bool vkdev::Buffer::createBuffer(const VkBufferUsageFlags& usage, 
                                 const VkMemoryPropertyFlags& properties, 
                                 const VkDeviceSize& size) {

  VkBufferCreateInfo buffer_ci {
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    nullptr,
    0,
    size,
    usage,                              
    VK_SHARING_MODE_EXCLUSIVE,
    0,
    nullptr
  };

  bufferSize_ = size;
  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  VkResult r = vkCreateBuffer(hdevice, 
                              &buffer_ci, 
                              nullptr, 
                              &bufferHandle_);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create vulkan buffer" << std::endl;
    return false;
  }

  VkMemoryRequirements buffer_mem_requirements;
  vkGetBufferMemoryRequirements(hdevice, 
                                bufferHandle_, 
                                &buffer_mem_requirements);

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(ResourceManager::GetResources()->hardDevice_, &memory_properties);

  for (size_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    //Check for minimum buffer memory requirements against physical device properties
    if ( (buffer_mem_requirements.memoryTypeBits & (1 << i)) && 
       (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      
      VkMemoryAllocateInfo allocate_info {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        buffer_mem_requirements.size,
        static_cast<uint32_t>(i)
      };

      VkResult r = vkAllocateMemory(hdevice, &allocate_info, nullptr, &bufferMemory_);
      if (r != VK_SUCCESS) {
        std::cout << "Couldn't allocate buffer memory" << std::endl;
        return false;
      }
    }
  }

  r = vkBindBufferMemory(hdevice, bufferHandle_, bufferMemory_, 0);
  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred binding buffer memory" << std::endl;
    return false;
  }

  return false;
}

/****************************************************************************/

bool vkdev::Buffer::mapBufferMemory(const void* data, const VkDeviceSize& size) {

  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  //InternalBuffer& ib = ResourceManager::GetResources()->userBuffers_[bufferId_];
  
  VkResult r = vkMapMemory(hdevice, bufferMemory_, 0, size, 0, &mapped_);
  if (r != VK_SUCCESS) {
    std::cout << "Could not map vertex buffer memory" << std::endl;
    return false;
  }

  //float* vertex_data = ResourceManager::GetResources()->vertexData_.data();
  memcpy(mapped_, data, size);

  VkMappedMemoryRange flush_range = {
    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
    nullptr,
    bufferMemory_,
    0,
    size
  };

  vkFlushMappedMemoryRanges(hdevice, 1, &flush_range);
  vkUnmapMemory(hdevice, bufferMemory_);
  
  return true;
}

/****************************************************************************/

bool vkdev::Buffer::mapGPUMemory() {
  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  
  VkResult r = vkMapMemory(hdevice, bufferMemory_, 0, bufferSize_, 0, &mapped_);
  if (r != VK_SUCCESS) {
    std::cout << "Could not map vertex buffer memory" << std::endl;
    return false;
  }
  return true;
}

/****************************************************************************/

void vkdev::Buffer::updateMappedMemory(const void* data, const VkDeviceSize& size, const size_t& offset) {
  auto mapped_offset = reinterpret_cast<uint64_t>(mapped_) + offset;
  auto data_offset = reinterpret_cast<uint64_t>(data) + offset;

  memcpy(reinterpret_cast<void*>(mapped_offset), reinterpret_cast<void*>(data_offset), size);
}

/****************************************************************************/

void vkdev::Buffer::FlushGPUMemory(const VkDeviceSize& size, const size_t& offset) {
  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  VkMappedMemoryRange flush_range = {
    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
    nullptr,
    bufferMemory_,
    offset,
    size
  };

  vkFlushMappedMemoryRanges(hdevice, 1, &flush_range);
}

/****************************************************************************/

void vkdev::Buffer::unmapGPUMemory(const VkDeviceSize& size, const size_t& offset) {
  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  FlushGPUMemory(size, offset);
  vkUnmapMemory(hdevice, bufferMemory_);
}

/****************************************************************************/

bool vkdev::Buffer::destroyBuffer() {
  const VkDevice& hdevice = ResourceManager::GetResources()->device_;
  if (bufferHandle_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(hdevice, bufferHandle_, nullptr);
    bufferHandle_ = VK_NULL_HANDLE;
  }

  if (bufferMemory_ != VK_NULL_HANDLE) {
    vkFreeMemory(hdevice, bufferMemory_, nullptr);
    bufferMemory_ = VK_NULL_HANDLE;
    return true;
  }

  return false;
}

/****************************************************************************/

const VkBuffer& vkdev::Buffer::getHandle() const {
  return bufferHandle_;
}

/****************************************************************************/

const uint64_t& vkdev::Buffer::getBufferSize() const {
  return bufferSize_;
}
