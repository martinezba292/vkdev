#ifndef __BUFFER_DATA_H__
#define __BUFFER_DATA_H__ 1

#include "common_def.h"

namespace vkdev {
class Buffer {
  public:
    Buffer();
    Buffer(const Buffer& buffer);
    ~Buffer(){}
    // const uint32_t getId() const;
    // bool loadData(const float* data, const uint32_t& n_data);

    bool createBuffer(const VkBufferUsageFlags& usage, 
                      const VkMemoryPropertyFlags& properties, 
                      const VkDeviceSize& size);
    bool mapBufferMemory(const void* data, const VkDeviceSize& size);
    bool mapGPUMemory();
    void updateMappedMemory(const void* data, const VkDeviceSize& size, const size_t& offset = 0);
    void FlushGPUMemory(const VkDeviceSize& size, const size_t& offset = 0);
    void unmapGPUMemory(const VkDeviceSize& size, const size_t& offset = 0);
    bool destroyBuffer();

    const VkBuffer& getHandle() const;
    const uint64_t& getBufferSize() const;

  private:
    VkBuffer bufferHandle_;
    VkDeviceMemory bufferMemory_;
    VkDeviceSize bufferSize_;
    void* mapped_;
 };
}
#endif