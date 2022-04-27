#include "commands/cmd_bindbuffer.h"
#include "internal_resources.h"
#include "vkinterface.h"
#include <cassert>


vkdev::BindBufferCmd::BindBufferCmd(uint64_t offset, uint32_t elements) :
handle_(ResourceManager::GetResources()->vertexBufferMemory_->getHandle()),
bindOffset_(offset),
count_(elements)
{
  type_ = CommandType_BindVertexBuffer;
}

void vkdev::BindBufferCmd::record(const VkCommandBuffer& buffer) const {
  VkDeviceSize offset = 0;

  vkCmdBindVertexBuffers(buffer, 0, 1, &handle_, &offset);
};