#include "vkbuffer.h"
#include "internal_resources.h"
#include "vkdevice.h"
#include "string.h"
#include <iostream>


vkdev::VertexBuffer::VertexBuffer() {
    bufferId_ = -1;
    bufferSize_ = 0;
    bufferHandle_ = VK_NULL_HANDLE;
}


vkdev::VertexBuffer::~VertexBuffer() {

}

bool vkdev::VertexBuffer::loadData(const float* data, const uint32_t& n_data) {
  if (!data) return false;

  bufferId_ = ResourceManager::GetResources()->vertexData_.size();
  ResourceManager::GetResources()->vertexData_.emplace_back(std::vector<float>(n_data));
  auto test = ResourceManager::GetResources()->vertexData_.size();
  auto last = &ResourceManager::GetResources()->vertexData_.back();
  last->reserve(n_data);
  bufferSize_ = sizeof(float) * n_data;
  void* r = memcpy(last->data(), data, bufferSize_);
  if (!r) {
    std::cout << "Couldn't copy vertex data to container" << std::endl;
    return false;
  }

  return true;
}

bool vkdev::VertexBuffer::createVertexBuffer(const Device& device) {
  if (bufferId_ < 0) {
    std::cout << "Vertex data has not been initialized.\
                  Couldn't create vertex data" << std::endl;
    return false;
  }

  VkDeviceSize d_size = bufferSize_;
  VkBufferCreateInfo buffer_ci {
    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    nullptr,
    0,
    static_cast<VkDeviceSize>(bufferSize_),
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  //Buffer usage for vertex data
    VK_SHARING_MODE_EXCLUSIVE,          //Used by only one queue at a time
    0,                                  //number of queues that will access buffer
    nullptr                             //indices of all queues that will access buffer
  };

  const VkDevice& hdevice = device.getDeviceHandle();
  VkResult r = vkCreateBuffer(hdevice, 
                              &buffer_ci, 
                              nullptr, 
                              &bufferHandle_);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create vulkan buffer" << std::endl;
    return false;
  }

  if (!allocateBufferMemory(device)) {
    return false;
  }

  r = vkBindBufferMemory(hdevice, bufferHandle_, bufferMemory_, 0);
  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred binding buffer memory" << std::endl;
    return false;
  }

  void* mapped_memory;
  r = vkMapMemory(hdevice, bufferMemory_, 0, bufferSize_, 0, &mapped_memory);
  if (r != VK_SUCCESS) {
    std::cout << "Could not map vertex buffer memory" << std::endl;
    return false;
  }

  float* vertex_data = ResourceManager::GetResources()->vertexData_[bufferId_].data();
  memcpy(mapped_memory, vertex_data, bufferSize_);

  VkMappedMemoryRange flush_range = {
    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
    nullptr,
    bufferMemory_,
    0,
    VK_WHOLE_SIZE
  }

  
  return true;
}


bool vkdev::VertexBuffer::allocateBufferMemory(const Device& device) {
  VkMemoryRequirements buffer_mem_requirements;
  vkGetBufferMemoryRequirements(device.getDeviceHandle(), 
                                bufferHandle_, 
                                &buffer_mem_requirements);

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(device.getCurrentPhysicalDevice(), &memory_properties);

  for (size_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    //Check for minimum buffer memory requirements against physical device properties
    if ( (buffer_mem_requirements.memoryTypeBits & (1 << i)) && 
       (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
      
      VkMemoryAllocateInfo allocate_info {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        buffer_mem_requirements.size,
        i
      };

      VkResult r = vkAllocateMemory(device.getDeviceHandle(), &allocate_info, nullptr, &bufferMemory_);
      if (r == VK_SUCCESS) {
        return true;
      }
    }
  }

  std::cout << "Couldn't allocate buffer memory" << std::endl;
  return false;
}


bool vkdev::VertexBuffer::destroyVertexBuffer() {


    return true;
}

