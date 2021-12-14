#include "vkinstance.h"
#include "vkloader.h"
#include "vkfunctions.h"
#include "vkhelpers.h"
#include <vector>
#include <stdexcept>
#include <iostream>

vkdev::Instance::Instance() 
{
  instance_ = VK_NULL_HANDLE;
  createInstance("Vulkan Application");
}

vkdev::Instance::Instance(const char* app_name) 
{
  createInstance(app_name);
}

vkdev::Instance::~Instance()
{
  if (instance_) {
    vkDestroyInstance(instance_, nullptr);
    instance_ = VK_NULL_HANDLE;
  }
}

VkInstance& vkdev::Instance::get() 
{
  return instance_;
}

bool vkdev::Instance::createInstance(const char* name)
{
  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = name;
  app_info.pEngineName = "No Engine";
  app_info.apiVersion = VK_API_VERSION_1_0;

  uint32_t ext_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(ext_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, available_extensions.data());

  std::vector<const char*> instance_extensions{ 
    VK_KHR_SURFACE_EXTENSION_NAME, 
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined VK_USE_PLATFORM_XLIB_KHR
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined VK_USE_PLATFORM_XCB_KHR
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
  };

  for (auto extension : instance_extensions) {
    if (!checkExtensionSupport(extension, available_extensions)) {
      std::cout << "Extension with name " << extension << " is not supported by instance" << std::endl;
      return false;
    }
  }

  VkInstanceCreateInfo instance_create_info = {
  VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType            sType
  nullptr,                                            // const void*                pNext
  0,                                                  // VkInstanceCreateFlags      flags
  &app_info,                                          // const VkApplicationInfo   *pApplicationInfo
  0,                                                  // uint32_t                   enabledLayerCount
  nullptr,                                            // const char * const        *ppEnabledLayerNames
  static_cast<uint32_t>(instance_extensions.size()),  // uint32_t                   enabledExtensionCount
  instance_extensions.data()
  };

  if (vkCreateInstance(&instance_create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance");
    return false;
  }

  if (!LoadInstanceLevelEntryPoints(instance_)) {
    throw std::runtime_error("failed load instance level functions");
    return false;
  }

  return true;
}
