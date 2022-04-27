#include "commands/cmd_binddescriptor.h"
#include "pipeline_resources.h"
#include "pipeline_descriptor.h"
#include "vkfunctions.h"

void vkdev::BindDescriptorCmd::record(const VkCommandBuffer& cmd) const {
    const VkDescriptorSet* desc_set = descSet_->getDescriptorSets();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->getLayout(), 
                            firstSet_, 1, &desc_set[firstSet_], 1, &offset_);
}