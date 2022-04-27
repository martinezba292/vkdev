#include "vkinstance.h"
#include "vkloader.h"
#include "vkfunctions.h"
#include "vkhelpers.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

vkdev::Instance::Instance() 
{
  instance_ = VK_NULL_HANDLE;
  debugMessenger_ = VK_NULL_HANDLE;
  validationLayers_ = false;
  createInstance("Vulkan Application");
}

vkdev::Instance::Instance(const char* app_name, bool enableDebugMessages) 
{
  validationLayers_ = enableDebugMessages;
  createInstance(app_name);
}

vkdev::Instance::~Instance()
{
  destroyInstance();
}

bool vkdev::Instance::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    if (!layerFound) {
        return false;
    }
  }
  return true;
}

void vkdev::Instance::setupDebugMessenger() {
  if (!validationLayers_) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr; // Optional

  // if (vkCreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS) {
  //   std::cout << "Failed to create debug messenger" << std::endl;
  // }

}

const VkInstance& vkdev::Instance::get() const 
{
  return instance_;
}

void vkdev::Instance::destroyInstance() {
  if (instance_) {
    vkDestroyInstance(instance_, nullptr);
    instance_ = VK_NULL_HANDLE;
  }
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

  if (validationLayers_ && !checkValidationLayerSupport()) {
      std::cout << "validation layers requested, but not available!" << std::endl;
  }
  
  if (validationLayers_)
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  //auto extensions = getRequiredExtensions(validationLayers_);
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

  if (validationLayers_) {
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());  
    instance_create_info.ppEnabledLayerNames = validationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&createInfo;
  }

  if (vkCreateInstance(&instance_create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance");
    return false;
  }

  if (!LoadInstanceLevelEntryPoints(instance_)) {
    throw std::runtime_error("failed load instance level functions");
    return false;
  }

  setupDebugMessenger();

  return true;
}
