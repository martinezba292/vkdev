#include "renderpass.h"
#include "vkfunctions.h"
#include "vkswapchain.h"
#include "vkdevice.h"
#include "vkhelpers.h"
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

    depthFormat_ = findSupportedFormats({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                          VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    std::vector<VkAttachmentDescription> attach_descriptors {
      {
        0,
        //Should match swapchain images format
        static_cast<VkFormat>(attachments.getImageFormat()),
        //Used for multisampling
        VK_SAMPLE_COUNT_1_BIT,
        //What to do when RP begins
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        //What to do when RP ends
        VK_ATTACHMENT_STORE_OP_STORE,
        //The same if attachment has stencil(CURRENTLY NOT USING)
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //Image layout at the begining/end of RP
        //Right now only using swapchain images for presentation
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      },
      {
        0,
        //Should match swapchain images format
        depthFormat_,
        //Used for multisampling
        VK_SAMPLE_COUNT_1_BIT,
        //What to do when RP begins
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        //What to do when RP ends
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //The same if attachment has stencil(CURRENTLY NOT USING)
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VK_ATTACHMENT_STORE_OP_DONT_CARE,
        //Image layout at the begining/end of RP
        //Right now only using swapchain images for presentation
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
      }
    };

    //Attachment reference references an index inside the VkAttachmentDescription array(currently only using one)
    std::vector<VkAttachmentReference> attach_references {
      {
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      },
      {
        1,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
      }
    };

    //Subpasses in the current renderpass
    VkSubpassDescription subpass_desc {
      0,                                //flags
      VK_PIPELINE_BIND_POINT_GRAPHICS,  //VkPipelineBindPoint
      0,                                //uint32_t inputAttachmentCount
      nullptr,                          //const VkAttachmentReference* pInputAttachments
      1,                                //uint32_t colorAttachmentCount
      &attach_references[0],                //const VkAttachmentReference* pColorAttachments
      nullptr,                          //const VkAttachmentReference* pResolveAttachments
      &attach_references[1],                          //const VkAttachmentReference* pDepthStencilAttachment
      0,                                //uint32_t preserveAttachmentCount
      nullptr                           //const uint32_t* pPreserveAttachments
    };

    std::vector<VkSubpassDependency> dependencies {
      {
        VK_SUBPASS_EXTERNAL,                            //uint32_t srcSubpass
        0,                                              //uint32_t dstSubpass
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           //VkPipelineStageFlags srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,  //VkPipelineStageFlags dstStageMask
        VK_ACCESS_MEMORY_READ_BIT,                      //VkAccessFlags srcAccessMask
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,           //VkAccessFlags dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT                     //VkDependencyFlags dependencyFlags
      },
      {
        0,
        VK_SUBPASS_EXTERNAL,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_DEPENDENCY_BY_REGION_BIT
      }
    };

    VkRenderPassCreateInfo renderpass_ci {
      VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      nullptr,      
      0,                                          //flags
      static_cast<uint32_t>(attach_descriptors.size()),                                          //uint32_t attachment_count
      &attach_descriptors[0],                              //const VkAttachmentDescription* pAttachments
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