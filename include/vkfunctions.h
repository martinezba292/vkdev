#ifndef __VK_FUNCTIONS__
#define __VK_FUNCTIONS__ 1
#include "vulkan/vulkan.h"

namespace vkdev {
#define VK_EXPORTED_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;

#include "functionlist.inl"
}


#endif