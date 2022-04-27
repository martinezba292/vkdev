#ifndef __INTERNAL_RESOURCES_H__
#define __INTERNAL_RESOURCES_H__ 1

#include <memory>
#include <vector>
#include "common_def.h"
#include "vkbuffer.h"

#define VERTEX_CAPACITY 30000
#define BUFFER_CAPACITY 256

namespace vkdev {
  struct DeviceQueue {
    int16_t queueFamily_;
    int16_t queueCount_;
    VkQueue queue_;
  };

  struct TextureData {
    std::string texturePath_;
    VkImage img_;
    VkImageView imgView_;
    VkSampler sampler_;
    VkDeviceMemory imgMemory_;
  };

  class CommandBuffer;
  class Buffer;
  class ResourceManager {
    struct InternalResources {
      DeviceHandle device_;
      PhysicalDevice hardDevice_;

      std::unique_ptr<DeviceQueue> graphicsQueue_;
      std::unique_ptr<DeviceQueue> presentQueue_;

      //Buffer for drawing
      std::unique_ptr<Buffer> vertexBufferMemory_;
      std::vector<float> vertexData_;

      std::unique_ptr<Buffer> indexBufferMemory_;
      std::vector<uint32_t> indexData_;

      std::vector<TextureData> textures_;
      std::unique_ptr<TextureData> depthResources_;

    };

    public:
      static inline std::shared_ptr<InternalResources> GetResources() {
        auto instance = Resources_;
        if (!instance) {
          instance = std::make_shared<InternalResources>();

          instance->device_ = VK_NULL_HANDLE;
          instance->hardDevice_ = VK_NULL_HANDLE;

          instance->vertexData_.reserve(VERTEX_CAPACITY);

          instance->graphicsQueue_ = std::make_unique<DeviceQueue>();
          instance->presentQueue_ = std::make_unique<DeviceQueue>();
          
          Resources_ = instance;
        }
        return instance;
      }
      
      static bool CreateVertexBuffers(CommandBuffer& cmd);
      static bool CreateTextures(CommandBuffer& cmd);
      static void FreeResources();


    private:
      static std::shared_ptr<InternalResources> Resources_;
  };
}

#endif