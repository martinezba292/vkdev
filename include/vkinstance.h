/*
Author: Manuel Martinez Barreiro
E-mail: martinezba292@gmail.com
*/

#ifndef __VK_INSTANCE__
#define __VK_INSTANCE__ 1

#include "vulkan/vulkan.h"

namespace vkdev {
  class Instance {
  public:
    Instance();
    Instance(const char* app_name);
    ~Instance();

    const VkInstance& get() const;

  private:
    VkInstance instance_;
    Instance(const Instance& instance);
    bool createInstance(const char* name);

  };
}


#endif