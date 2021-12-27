/*
Author: Manuel Martinez Barreiro
E-mail: martinezba292@gmail.com
*/

#ifndef __VK_LOADER__
#define __VK_LOADER__ 1

#include "vulkan/vulkan.h"

namespace vkdev {
    bool PrepareVulkan();
    bool CloseVulkan();

    bool LoadVulkanLibrary();
    bool LoadExportedEntryPoints();
    bool LoadGlobalLevelEntryPoints();
    bool LoadInstanceLevelEntryPoints(const VkInstance& instance);
    bool LoadDeviceLevelEntryPoints(const VkDevice& device);
}

#endif