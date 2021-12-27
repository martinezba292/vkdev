#include "commands/vkclear.h"
#include "command_buffer.h"
#include "vkfunctions.h"

vkdev::ClearCmd::ClearCmd(const float* rgba, const VkImage& render_img, const uint32_t queue_family) {
    type_ = CommandType_Clear;
    clearColor_ = {rgba[0], rgba[1], rgba[2], rgba[3]};
    clearImg_ = &render_img;
    queueFamily_ = queue_family;
}

vkdev::ClearCmd::ClearCmd(const ClearCmd& cmd) {
    type_ = cmd.type_;
    clearColor_ = cmd.clearColor_;
    clearImg_ = cmd.clearImg_;
    queueFamily_ = cmd.queueFamily_;
}


vkdev::ClearCmd::~ClearCmd(){

}

void vkdev::ClearCmd::record(const VkCommandBuffer& buffer) const {
    VkImageSubresourceRange subresource_range{};
    subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.layerCount = 1;

    VkImageMemoryBarrier present_to_clear{};
    present_to_clear.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    present_to_clear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    present_to_clear.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    present_to_clear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    present_to_clear.srcQueueFamilyIndex = queueFamily_;
    present_to_clear.dstQueueFamilyIndex = queueFamily_;
    present_to_clear.image = *clearImg_;
    present_to_clear.subresourceRange = subresource_range;

    VkImageMemoryBarrier clear_to_present{};
    present_to_clear.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    present_to_clear.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    present_to_clear.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    present_to_clear.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    present_to_clear.srcQueueFamilyIndex = queueFamily_;
    present_to_clear.dstQueueFamilyIndex = queueFamily_;
    present_to_clear.image = *clearImg_;
    present_to_clear.subresourceRange = subresource_range;

    vkCmdPipelineBarrier(buffer, 
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &present_to_clear);

    vkCmdClearColorImage(buffer, *clearImg_, 
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                         &clearColor_, 1, &subresource_range);

    vkCmdPipelineBarrier(buffer, 
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &clear_to_present);

}