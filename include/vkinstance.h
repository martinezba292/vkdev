#ifndef __VK_INSTANCE__
#define __VK_INSTANCE__ 1

#include "vulkan/vulkan.h"

namespace vkdev {
  class Instance {
  public:
    Instance();
    Instance(const char* app_name);
    ~Instance();

    VkInstance& get();

  private:
    VkInstance instance_;
    Instance(const Instance& instance);
    void createInstance(const char* name);

  };
}


#endif