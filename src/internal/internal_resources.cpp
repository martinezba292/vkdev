#include "internal_resources.h"
#include <iostream>
#include "vkbuffer.h"
#include "command_buffer.h"
#include "vkhelpers.h"
#include "vkfunctions.h"
#include "lodepng.h"
#include "glm.hpp"
//#include "vkbuffer.h"

std::shared_ptr<vkdev::ResourceManager::InternalResources> vkdev::ResourceManager::Resources_ = nullptr;

/***********************************************************************************/

bool vkdev::ResourceManager::CreateVertexBuffers(CommandBuffer& cmd) {
  if (Resources_->vertexBufferMemory_) {
    std::cout << "Vertex buffers already been created" << std::endl;
    return false;
  }

  Buffer staging_buffer;
  VkDeviceSize buffer_size = ResourceManager::GetResources()->vertexData_.size() * sizeof(float);

  float* vertex_data = &Resources_->vertexData_[0];
  staging_buffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, buffer_size);

  Resources_->vertexBufferMemory_ = std::make_unique<Buffer>();
  auto mbuffer = Resources_->vertexBufferMemory_.get();
  mbuffer->createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_size);

  staging_buffer.mapBufferMemory(vertex_data, buffer_size);


  Buffer staging_index;
  VkDeviceSize index_size = ResourceManager::GetResources()->indexData_.size() * sizeof(uint32_t);

  uint32_t* index_data = &Resources_->indexData_[0];
  staging_index.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, index_size);

  Resources_->indexBufferMemory_ = std::make_unique<Buffer>();
  auto ibuffer = Resources_->indexBufferMemory_.get();
  ibuffer->createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_size);

  staging_index.mapBufferMemory(index_data, index_size);


  VkCommandBufferBeginInfo cmd_begin_ci {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    nullptr,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    nullptr
  };

  const VkCommandBuffer& cmdhandle = cmd.getCommandBuffer();
  vkBeginCommandBuffer(cmdhandle, &cmd_begin_ci);
  VkBufferCopy buffer_cp_info {
    0,
    0,
    buffer_size
  };

  const VkBuffer& bufferh = mbuffer->getHandle();
  vkCmdCopyBuffer(cmdhandle, staging_buffer.getHandle(), bufferh, 1, &buffer_cp_info);

  buffer_cp_info.size = index_size;
  vkCmdCopyBuffer(cmdhandle, staging_index.getHandle(), ibuffer->getHandle(), 1, &buffer_cp_info);

  std::vector<VkBufferMemoryBarrier> buffer_memory_barrier {
   { 
    VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,          // VkStructureType                        sType;
    nullptr,                                          // const void                            *pNext
    VK_ACCESS_MEMORY_WRITE_BIT,                       // VkAccessFlags                          srcAccessMask
    VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,              // VkAccessFlags                          dstAccessMask
    VK_QUEUE_FAMILY_IGNORED,                          // uint32_t                               srcQueueFamilyIndex
    VK_QUEUE_FAMILY_IGNORED,                          // uint32_t                               dstQueueFamilyIndex
    bufferh,                           // VkBuffer                               buffer
    0,                                                // VkDeviceSize                           offset
    VK_WHOLE_SIZE                                     // VkDeviceSize                           size
   },
   { 
    VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,          // VkStructureType                        sType;
    nullptr,                                          // const void                            *pNext
    VK_ACCESS_MEMORY_WRITE_BIT,                       // VkAccessFlags                          srcAccessMask
    VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,              // VkAccessFlags                          dstAccessMask
    VK_QUEUE_FAMILY_IGNORED,                          // uint32_t                               srcQueueFamilyIndex
    VK_QUEUE_FAMILY_IGNORED,                          // uint32_t                               dstQueueFamilyIndex
    ibuffer->getHandle(),                           // VkBuffer                               buffer
    0,                                                // VkDeviceSize                           offset
    VK_WHOLE_SIZE                                     // VkDeviceSize                           size
   }
  };

  vkCmdPipelineBarrier(cmdhandle, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
                       0, 0, nullptr, static_cast<uint32_t>(buffer_memory_barrier.size()), 
                       &buffer_memory_barrier[0], 0, nullptr);
  
  vkEndCommandBuffer(cmdhandle);

  VkSubmitInfo submit_info {
    VK_STRUCTURE_TYPE_SUBMIT_INFO,
    nullptr,
    0,
    nullptr,
    nullptr,
    1,
    &cmdhandle,
    0,
    nullptr
  };

  VkQueue& q = ResourceManager::GetResources()->graphicsQueue_->queue_;

  VkResult r = vkQueueSubmit(q, 1, &submit_info, VK_NULL_HANDLE);
  if (r != VK_SUCCESS) {
    std::cout << "Error ocurred during buffer copy operations" << std::endl;
    return false;
  }

  vkQueueWaitIdle(q);
  staging_buffer.destroyBuffer();
  staging_index.destroyBuffer();

  return true;
}

/***********************************************************************************/

bool vkdev::ResourceManager::CreateTextures(CommandBuffer& cmd) {

  const VkDevice& hdevice = ResourceManager::GetResources()->device_;

  //DEPTH RESOURCES
  VkFormat format = findSupportedFormats({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                          VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

  Resources_->depthResources_ = std::make_unique<TextureData>();
  TextureData* depth = (Resources_->depthResources_.get());
  createImage(depth->img_, WINDOW_W, WINDOW_H, VK_IMAGE_TYPE_2D, format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  allocateImageMemory(depth->imgMemory_, depth->img_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkBindImageMemory(hdevice, depth->img_, depth->imgMemory_, 0);
  //const VkDevice device = ResourceManager::GetResources()->device_;
  createImageView(hdevice, format, depth->img_, depth->imgView_, VK_IMAGE_ASPECT_DEPTH_BIT);


  if (Resources_->textures_.empty()) return false;

  struct TextureCopy {
    VkDeviceSize total_size = 0;
    std::vector<unsigned char> texture_buffer;
    std::vector<VkDeviceSize> offsets;
    std::vector<glm::uvec2> extent;
  }Textcp;

  Textcp.texture_buffer.reserve(100000000);
  for (auto& t : Resources_->textures_) {
    std::vector<unsigned char> texture_data;
    uint32_t w, h;
    if (lodepng::decode(texture_data, w, h, t.texturePath_)) {
      std::cout << "Failed to load texture data" << std::endl;
      return false;
    }

    Textcp.texture_buffer.insert(Textcp.texture_buffer.end(), texture_data.begin(), texture_data.end());
    Textcp.offsets.push_back(Textcp.total_size);
    Textcp.total_size += (w * h) * sizeof(float);
    Textcp.extent.push_back({w, h});
  }

  Buffer staging_buffer;

  staging_buffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, Textcp.total_size);
  staging_buffer.mapBufferMemory(&Textcp.texture_buffer[0], Textcp.total_size);


  VkResult r;

  VkCommandBufferBeginInfo cmd_begin_ci {
    VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    nullptr,
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    nullptr
  };


  auto ext = Textcp.extent.begin();
  auto offset = Textcp.offsets.begin();
  const VkCommandBuffer& cmdhandle = cmd.getCommandBuffer();
  vkBeginCommandBuffer(cmdhandle, &cmd_begin_ci);
  for (auto& t : Resources_->textures_) {
    createImage(t.img_, ext->x, ext->y, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, 
                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    allocateImageMemory(t.imgMemory_, t.img_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkBindImageMemory(hdevice, t.img_, t.imgMemory_, 0);
    createImageView(hdevice, VK_FORMAT_R8G8B8A8_UNORM, t.img_, t.imgView_);
    createSampler(t.sampler_, VK_FILTER_LINEAR);

      VkImageSubresourceRange img_subresource_range {
      VK_IMAGE_ASPECT_COLOR_BIT,
      0,
      1,
      0,
      1
    };

    VkImageMemoryBarrier undefined_to_transfer_dst {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType           sType
      nullptr,                                // const void               *pNext                                     
      0,                                      // VkAccessFlags             srcAccessMask
      VK_ACCESS_TRANSFER_WRITE_BIT,           // VkAccessFlags             dstAccessMask
      VK_IMAGE_LAYOUT_UNDEFINED,              // VkImageLayout             oldLayout
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // VkImageLayout             newLayout
      VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  srcQueueFamilyIndex
      VK_QUEUE_FAMILY_IGNORED,                // uint32_t                  dstQueueFamilyIndex
      t.img_,                                   // VkImage                   image
      img_subresource_range                   // VkImageSubresourceRange   subresourceRange
    };

    vkCmdPipelineBarrier(cmdhandle, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 
                         0, nullptr, 1, &undefined_to_transfer_dst);

    VkBufferImageCopy image_copy {
      *offset,
      0,
      0,
      {
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        0,
        1
      },
      {
        0,
        0,
        0
      },
      {
        ext->x,
        ext->y,
        1
      }
    };

    vkCmdCopyBufferToImage(cmdhandle, staging_buffer.getHandle(), t.img_, 
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                           1, &image_copy);

    VkImageMemoryBarrier img_mem_barrier {
      VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      nullptr,
      VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_ACCESS_SHADER_READ_BIT,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      t.img_,
      img_subresource_range
    };

    vkCmdPipelineBarrier(cmdhandle, VK_PIPELINE_STAGE_TRANSFER_BIT, 
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 
                         nullptr, 0, nullptr, 1, &img_mem_barrier);

    ext++;
    offset++;

  }

  vkEndCommandBuffer(cmdhandle);

  VkSubmitInfo submit_info {
    VK_STRUCTURE_TYPE_SUBMIT_INFO,
    nullptr,
    0,
    nullptr,
    nullptr,
    1,
    &cmdhandle,
    0,
    nullptr
  };

  VkQueue& q = ResourceManager::GetResources()->graphicsQueue_->queue_;
  if (vkQueueSubmit(q, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    std::cout << "Failed to submit image copy to queue" << std::endl;
    return false;
  }

  vkQueueWaitIdle(q);
  staging_buffer.destroyBuffer();

  return true;
}

/***********************************************************************************/

void vkdev::ResourceManager::FreeResources() {
  Resources_->vertexBufferMemory_->destroyBuffer();
  Resources_->indexBufferMemory_->destroyBuffer();

  //Resources_->userBuffers_.clear();
  Resources_->vertexData_.clear();
  Resources_->indexData_.clear();

  for (auto& t : Resources_->textures_) {
    destroyTexture(t);
  }
  destroyTexture(*Resources_->depthResources_);
  Resources_->graphicsQueue_.reset();
  Resources_->presentQueue_.reset();
  Resources_->vertexBufferMemory_.reset();
}

/***********************************************************************************/