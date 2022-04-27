#include "pipeline_descriptor.h"
#include "internal_resources.h"
#include <iostream>
#include "vktexture.h"
#include "vkfunctions.h"
#include <array>

#define MAX_POOL_SETS 10
#define MAX_LAYOUTS 5

vkdev::PipelineDescriptor::PipelineDescriptor() {
  descriptorList_.reserve(MAX_POOL_SETS);
}

/**************************************************************************/

vkdev::PipelineDescriptor::~PipelineDescriptor() {

}

/**************************************************************************/

bool vkdev::PipelineDescriptor::createSimpleLayout(const DescriptorType& type, const VkShaderStageFlagBits& stage) {

  VkDescriptorSetLayoutBinding layout_binding 
  { 
    0,
    static_cast<VkDescriptorType>(type),
    1,
    stage,
    nullptr
  };

  VkDescriptorSetLayoutCreateInfo set_layout_ci {
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    nullptr,
    0,
    1,
    &layout_binding
  };

  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  DescriptorResources resources;
  VkResult r = vkCreateDescriptorSetLayout(deviceh, &set_layout_ci, nullptr, &resources.setLayout);
  if (r != VK_SUCCESS) {
    std::cout << "Failed to create descriptor set layout" << std::endl;
    return false;
  }

  VkDescriptorPoolSize pool_size {
    layout_binding.descriptorType,
    MAX_POOL_SETS
  };

  VkDescriptorPoolCreateInfo descriptor_pool_ci {
    VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    nullptr,
    0,
    MAX_POOL_SETS,
    1,
    &pool_size
  };

  r = vkCreateDescriptorPool(deviceh, &descriptor_pool_ci, nullptr, &resources.descriptorPool);
  if (r != VK_SUCCESS) {
    std::cout << "Couldn't create descriptor pool" << std::endl;
    vkDestroyDescriptorSetLayout(deviceh, resources.setLayout, nullptr);
    return false;
  }

  descriptorMap_.insert({type, resources});
  return true;
}

/**************************************************************************/

uint32_t vkdev::PipelineDescriptor::allocateDescriptorSetFromLayoutType(const DescriptorType& type) {
  const DescriptorResources& resources = descriptorMap_[type];
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  VkDescriptorSetAllocateInfo descriptor_allocate_info {
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    nullptr,
    resources.descriptorPool,
    1,
    &resources.setLayout
  };

  VkDescriptorSet descriptor_set;
  VkResult r = vkAllocateDescriptorSets(deviceh, &descriptor_allocate_info, &descriptor_set);
  if (r != VK_SUCCESS) {
    std::cout << "An error ocurred during descriptor set allocation. Descriptor pool have allocated " << 
                descriptorList_.size() << " descriptors at this time." << std::endl;
    return 0;
  }

  descriptorList_.push_back(std::move(descriptor_set));

  return descriptorList_.size();
}

/**************************************************************************/

bool vkdev::PipelineDescriptor::updateDescriptorSet(const Texture& texture, const uint32_t& index_set, const uint32_t& slot_bind) {
  const TextureData& tdata = ResourceManager::GetResources()->textures_[texture.getId()];

  VkDescriptorImageInfo img_info {
    tdata.sampler_,
    tdata.imgView_,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
  };

  VkWriteDescriptorSet descriptor_write {
    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    nullptr,
    descriptorList_[index_set],
    slot_bind,
    0,
    1,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    &img_info,
    nullptr,
    nullptr
  };

  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  vkUpdateDescriptorSets(deviceh, 1, &descriptor_write, 0, nullptr);

  return true;
}

/**************************************************************************/

bool vkdev::PipelineDescriptor::updateDescriptorSet(const Buffer& buffer, const DescriptorType& buffer_type, 
                                                    const uint32_t& index_set, const VkDeviceSize& range) {
  uint32_t offset = 0;
  if (range != VK_WHOLE_SIZE) offset = range;
  VkDescriptorBufferInfo buffer_info {
    buffer.getHandle(),
    0,
    range,
  };


  VkWriteDescriptorSet descriptor_write {
    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    nullptr,
    descriptorList_[index_set],
    0, //slot_bind
    0,
    1,
    static_cast<VkDescriptorType>(buffer_type),
    nullptr,
    &buffer_info,
    nullptr
  };

  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  vkUpdateDescriptorSets(deviceh, 1, &descriptor_write, 0, nullptr);

  return true;
}

bool vkdev::PipelineDescriptor::updateDescriptorBuffers(const Buffer* buffer, const VkDeviceSize* ranges) {
  if (buffer == nullptr || ranges == nullptr) {
    std::cout << "Failed to update descriptors. Null references"  << std::endl;
    return false;
  }

  size_t size = descriptorMap_.size();
  std::vector<VkWriteDescriptorSet> descriptor_write(size);
  std::vector<VkDescriptorBufferInfo> buffer_info(size);
  size_t i = 0;
  for (auto& descriptor_type : descriptorMap_) {
    buffer_info[i] = {
      buffer[i].getHandle(),
      0,
      ranges[i]
    };

    descriptor_write[i] = {
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      nullptr,
      descriptorList_[i],
      0, //slot_bind
      0,
      1,
      static_cast<VkDescriptorType>(descriptor_type.first),
      nullptr,
      &buffer_info[i],
      nullptr
    };

    i++;
  }

  //NOTE: Descriptors of same pipeline layout must be updated at the same time in a DescriptorWrite array
  //Calling vkUpdateDescriptorSets multiple times derives in alterations of the layout offsets in shader
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  vkUpdateDescriptorSets(deviceh, static_cast<uint32_t>(descriptor_write.size()), &descriptor_write[0], 0, nullptr);

  return true;
}

/**************************************************************************/

void vkdev::PipelineDescriptor::getDescriptorLayout(std::vector<VkDescriptorSetLayout>& v) const {
  const size_t s = descriptorMap_.size();
  v.resize(s);
  int32_t i = 0;
  for (auto& element : descriptorMap_) {
    v[i] = element.second.setLayout;
    i++;
  }
}

/**************************************************************************/

const uint32_t vkdev::PipelineDescriptor::layoutCount() const {
  return descriptorMap_.size();
}

/**************************************************************************/

const VkDescriptorSet* vkdev::PipelineDescriptor::getDescriptorSets() const {
  return &descriptorList_[0];
}

/**************************************************************************/

void vkdev::PipelineDescriptor::destroyPipelineDescriptor() {
  const VkDevice& deviceh = ResourceManager::GetResources()->device_;
  for (auto& m : descriptorMap_) {
    if (m.second.descriptorPool != VK_NULL_HANDLE) {
      vkDestroyDescriptorPool(deviceh, m.second.descriptorPool, nullptr);
      m.second.descriptorPool = VK_NULL_HANDLE;
      if (m.second.setLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(deviceh, m.second.setLayout, nullptr);
        m.second.setLayout = VK_NULL_HANDLE;
      }
    }
  }
  descriptorList_.clear();
  descriptorMap_.clear();
  //bindings_.clear();
}

/**************************************************************************/

// bool vkdev::PipelineDescriptor::createLayoutFromBindings() {
//   const uint32_t b_size = bindings_.size();
//   if (b_size == 0) {
//     std::cout << "Couldn't create descriptor set because no bindings were found" << std::endl;
//     return false;
//   }

//   std::vector<VkDescriptorPoolSize> pool_size(b_size);
//   int32_t i = 0;
//   for (auto& b : bindings_) {
//     pool_size[i] = {
//       b.descriptorType,
//       MAX_POOL_SETS
//     };
//     i++;
//   }

//   VkDescriptorSetLayoutCreateInfo set_layout_ci {
//     VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
//     nullptr,
//     0,
//     b_size,
//     &bindings_[0]
//   };

//   const VkDevice& deviceh = ResourceManager::GetResources()->device_;
//   DescriptorResources resources;
//   //VkDescriptorSetLayout set_layout;
//   VkResult r = vkCreateDescriptorSetLayout(deviceh, &set_layout_ci, nullptr, &resources.setLayout);
//   if (r != VK_SUCCESS) {
//     std::cout << "Failed to create descriptor set layout" << std::endl;
//     return false;
//   }

//   VkDescriptorPoolCreateInfo descriptor_pool_ci {
//     VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
//     nullptr,
//     0,
//     MAX_POOL_SETS,
//     b_size,
//     &pool_size[0]
//   };

//   //VkDescriptorPool pool;
//   r = vkCreateDescriptorPool(deviceh, &descriptor_pool_ci, nullptr, &resources.descriptorPool);
//   if (r != VK_SUCCESS) {
//     std::cout << "Couldn't create descriptor pool" << std::endl;
//     vkDestroyDescriptorSetLayout(deviceh, resources.setLayout, nullptr);
//     return false;
//   }

//   // setLayouts_.push_back(std::move(set_layout));
//   // descriptorPools_.push_back(std::move(pool));
//   descriptorMap_.insert({static_cast<DescriptorType>(bindings_[0].descriptorType), resources});
//   bindings_.clear();

//   return true;
// }


/**************************************************************************/

// void vkdev::PipelineDescriptor::addBinding(const uint32_t& binding, const DescriptorType& type, const VkShaderStageFlagBits& stage) {  

//   VkDescriptorSetLayoutBinding layout_binding 
//   { 
//     binding,
//     static_cast<VkDescriptorType>(type),
//     1,
//     stage,
//     nullptr
//   };

//   bindings_.push_back(std::move(layout_binding));
// }