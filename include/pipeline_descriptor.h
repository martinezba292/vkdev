#ifndef __PIPELINE_DESCRIPTOR_H__
#define __PIPELINE_DESCRIPTOR_H__ 1

#include "common_def.h"
#include <vector>
#include <map>

enum DescriptorType {
  DescriptorType_NONE = 0,
  DescriptorType_Sampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
  DescriptorType_UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
  DescriptorType_StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
  DescriptorType_DynamicUniform = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
  DescriptorType_DynamicStorage = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
};

namespace vkdev {
  struct DescriptorResources {
    VkDescriptorSetLayout setLayout;
    VkDescriptorPool descriptorPool;
  };
  class Texture;
  class Buffer;
  class PipelineDescriptor {
    public:
      PipelineDescriptor();
      ~PipelineDescriptor();

      bool createSimpleLayout(const DescriptorType& type, const VkShaderStageFlagBits& stage);
      uint32_t allocateDescriptorSetFromLayoutType(const DescriptorType& type);

      bool updateDescriptorSet(const Texture& texture, const uint32_t& index_set, const uint32_t& slot_bind);
      bool updateDescriptorSet(const Buffer& buffer, const DescriptorType& buffer_type, 
                               const uint32_t& index_set, const VkDeviceSize& range = VK_WHOLE_SIZE);
      bool updateDescriptorBuffers(const Buffer* buffer, const VkDeviceSize* ranges);
      void getDescriptorLayout(std::vector<VkDescriptorSetLayout>& v) const;
      const uint32_t layoutCount() const;
      const VkDescriptorSet* getDescriptorSets() const;
      void destroyPipelineDescriptor();


      // void addBinding(const uint32_t& binding, const DescriptorType& type, const VkShaderStageFlagBits& stage);
      // bool createLayoutFromBindings();

    private:
      std::map<DescriptorType, DescriptorResources> descriptorMap_;
      std::vector<VkDescriptorSet> descriptorList_;
      //std::vector<VkDescriptorSetLayoutBinding> bindings_;
  };
}

#endif