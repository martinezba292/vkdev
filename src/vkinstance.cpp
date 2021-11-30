#include "vkinstance.h"
#include "vkloader.h"
#include "vkfunctions.h"
#include <vector>

vkdev::Instance::Instance()
{
  instance_ = VK_NULL_HANDLE;
}

vkdev::Instance::~Instance()
{

}

void vkdev::Instance::createInstance(const char* name)
{
  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = name;
  app_info.pEngineName = "VkDev";
  app_info.apiVersion = VK_API_VERSION_1_0;

  std::vector<const char*> instance_extensions{ VK_KHR_SURFACE_EXTENSION_NAME };

#ifdef _WIN32
  instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined VK_USE_PLATFORM_XCB_KHR
  instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#endif

  uint32_t ext_count = 0;
  //vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);

  VkInstanceCreateInfo instance_create_info = {
  VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType
  nullptr,                                        // const void*                pNext
  0,                                              // VkInstanceCreateFlags      flags
  &app_info,                              // const VkApplicationInfo   *pApplicationInfo
  0,                                              // uint32_t                   enabledLayerCount
  nullptr,                                        // const char * const        *ppEnabledLayerNames
  0,                                              // uint32_t                   enabledExtensionCount
  nullptr                                         // const char * const        *ppEnabledExtensionNames
  };

  vkCreateInstance(&instance_create_info, nullptr, &instance_);
  //PFN_vkCreateInstance(&instance_create_info, nullptr, &instance_);
}
