/*
Author: Manuel Martinez Barreiro
E-mail: martinezba292@gmail.com
*/

#ifndef __VK_INSTANCE__
#define __VK_INSTANCE__ 1

#include "common_def.h"
#include <vector>


namespace vkdev {

  const std::vector<const char*> validationLayers = {
      "VK_LAYER_LUNARG_standard_validation"
  };

  class Instance {
  public:
    Instance();
    Instance(const char* app_name, bool enableDebugMessages = false);
    void destroyInstance();
    ~Instance();

    const VkInstance& get() const;

  private:
    Instance(const Instance& instance);
    bool createInstance(const char* name);
    void setupDebugMessenger();
    bool checkValidationLayerSupport();

  private:
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debugMessenger_;
    bool validationLayers_;

  };
}


#endif