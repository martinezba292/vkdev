#ifndef __VK_HELPERS__
#define __VK_HELPERS__ 1

#include "vulkan/vulkan.h"
#include <vector>
#include <string.h>

namespace vkdev {
    inline bool checkExtensionSupport(const char* extension, std::vector<VkExtensionProperties>& available_extensions) {
      for (auto ext : available_extensions) {
        if (strcmp(extension, ext.extensionName) == 0)
          return true;
      }

    return false;
    }

}

#endif