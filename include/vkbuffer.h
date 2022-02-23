#ifndef __BUFFER_DATA_H__
#define __BUFFER_DATA_H__ 1

#include "common_def.h"

namespace vkdev {
class Device;
class VertexBuffer {
    public:
      VertexBuffer();
      ~VertexBuffer();
       bool loadData(const float* data, const uint32_t& n_data);
       bool createVertexBuffer(const Device& device);
       bool destroyVertexBuffer(); 

    private:
      bool allocateBufferMemory(const Device& device);

      int32_t bufferId_;
      uint32_t bufferSize_;
      VkDeviceMemory bufferMemory_;
      VkBuffer bufferHandle_;
 };
}
#endif