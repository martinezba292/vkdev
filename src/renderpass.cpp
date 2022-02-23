#include "renderpass.h"
#include "vkfunctions.h"
#include "vkswapchain.h"
#include "vkdevice.h"
#include <iostream>

vkdev::RenderPass::RenderPass() {
    renderPass_ = VK_NULL_HANDLE;
    deviceOwner_ = nullptr;
}

vkdev::RenderPass::~RenderPass() {
  destroyRenderPass();
}


bool vkdev::RenderPass::createRenderPass(const Device& device, const SwapChain& attachments) {
    deviceOwner_ = &device.getDeviceHandle();
    //Attachment description specifies the kind of operations that will be done
    VkAttachmentDescription color_attach;
    color_attach.flags = 0;
    //Should match swapchain images format
    color_attach.format = static_cast<VkFormat>(attachments.getImageFormat());
    //Used for multisampling
    color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
    //What to do when RP begins
    color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //What to do when RP ends
    color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //The same if attachment has stencil(CURRENTLY NOT USING)
    color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //Image layout at the begining/end of RP
    //Right now only using swapchain images for presentation
    color_attach.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //Attachment reference references an index inside the VkAttachmentDescription array(currently only using one)
    VkAttachmentReference color_attach_ref;
    color_attach_ref.attachment = 0;
    color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //Subpasses in the current renderpass
    VkSubpassDescription subpass_desc {
      0,                                //flags
      VK_PIPELINE_BIND_POINT_GRAPHICS,  //VkPipelineBindPoint
      0,                                //uint32_t inputAttachmentCount
      nullptr,                          //const VkAttachmentReference* pInputAttachments
      1,                                //uint32_t colorAttachmentCount
      &color_attach_ref,                //const VkAttachmentReference* pColorAttachments
      nullptr,                          //const VkAttachmentReference* pResolveAttachments
      nullptr,                          //const VkAttachmentReference* pDepthStencilAttachment
      0,                                //uint32_t preserveAttachmentCount
      nullptr                           //const uint32_t* pPreserveAttachments
    };

    std::vector<VkSubpassDependency> dependencies {
      {
        VK_SUBPASS_EXTERNAL,                            //uint32_t srcSubpass
        0,                                              //uint32_t dstSubpass
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           //VkPipelineStageFlags srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  //VkPipelineStageFlags dstStageMask
        VK_ACCESS_MEMORY_READ_BIT,                      //VkAccessFlags srcAccessMask
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           //VkAccessFlags dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT                     //VkDependencyFlags dependencyFlags
      },
      {
        0,
        VK_SUBPASS_EXTERNAL,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_DEPENDENCY_BY_REGION_BIT
      }
    };

    VkRenderPassCreateInfo renderpass_ci {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      nullptr,      
      0,                                          //flags
      1,                                          //uint32_t attachment_count
      &color_attach,                              //const VkAttachmentDescription* pAttachments
      1,                                          //uint32_t subpass_count
      &subpass_desc,                              //const VkSubpassDescription* pSubpasses
      static_cast<uint32_t>(dependencies.size()), //uint32_t dependencyCount
      &dependencies[0]                            //const VkSubpassDependencies* pDependencies
    };
    
    VkResult r = vkCreateRenderPass(device.getDeviceHandle(), &renderpass_ci, nullptr, &renderPass_);
    if (r != VK_SUCCESS) {
        std::cout << "An error ocurred during renderpass creation" << std::endl;
        return false;
    }

    return true;
}

bool vkdev::RenderPass::destroyRenderPass() {
    if (renderPass_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(*deviceOwner_);
        vkDestroyRenderPass(*deviceOwner_, renderPass_, nullptr);
        renderPass_ = VK_NULL_HANDLE;
        deviceOwner_ = nullptr;
        return true;
    }
    return false;
}

const VkRenderPass& vkdev::RenderPass::getHandle() const {
  return renderPass_;
}

const DeviceHandle* vkdev::RenderPass::getDevice() const {
  return deviceOwner_;
}