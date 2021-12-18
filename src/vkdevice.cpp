#include "vkdevice.h"
#include "vkfunctions.h"
#include "vkloader.h"
#include "vkhelpers.h"
#include "vkinstance.h"
#include "window.h"
#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <memory>

vkdev::Device::Device()
{
  device_ = VK_NULL_HANDLE;
}

vkdev::Device::Device(Instance& instance, Window& window)
{
  device_ = VK_NULL_HANDLE;
  createDevice(instance, window);
}

vkdev::Device::~Device()
{
  if (device_ != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(device_);
    vkDestroyDevice(device_, nullptr);
    device_ = VK_NULL_HANDLE;
  }
}



namespace vkdev {
  //If queue family with present support is required, a window handle is needed in order to check for surface support
  Queue* getQueueFamilyWithProperties(const VkPhysicalDevice device, const int32_t support, 
                                       const uint16_t min_queue, Window* window_handle = nullptr) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    if (!queue_family_count) {
      std::cout << "Physical device" << device << "doesn't have any queue families!" << std::endl; 
      return nullptr;
    }

    std::vector<VkQueueFamilyProperties> family_prperties(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, family_prperties.data());

    typedef std::function<int32_t(int32_t)> lambda_expr;
    std::list<lambda_expr> l;
    if (support & SUPPORT_GRAPHICS_OP) {
      l.push_back(
        [&](int32_t i) {
        return (family_prperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT);
      });
    }

    if (support & SUPPORT_PRESENT_OP) {
      l.push_back(
        [&](int32_t i) {
          VkBool32 have_support = VK_FALSE;
          vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window_handle->getSurface(), &have_support);
        return have_support;
      });
    }

    uint32_t family_index = UINT32_MAX;
    uint32_t queue_count = UINT32_MAX;
    for (uint32_t i = 0; i < queue_family_count; i++) {
      int32_t required = family_prperties[i].queueCount >= min_queue;
      for (auto lambda : l) {
        required &= lambda(i);
      }
      if (required) {
        queue_count = family_prperties[i].queueCount;
        family_index = i;
      }
    }

    if (family_index == UINT32_MAX) return nullptr;
    

    Queue queue{};
    queue.queueCount_ = queue_count;
    queue.queueFamily_ = family_index;

    return std::make_shared<Queue>(queue).get();      
  }


  uint32_t checkPhysicalDeviceProperties(const VkPhysicalDevice& device, Window& window_surface, 
                                         uint32_t& queue_family, uint32_t& present_family) 
  {
    //Get device extension availables
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data()) != VK_SUCCESS) {
      std::cout << "Error enumerating physical device" << device << "extension properties" << std::endl;
      return false;
    }

    //Check for Swapchain extension support
    std::vector<const char*> extensions_needed = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (auto extension : extensions_needed) {
      if (!checkExtensionSupport(extension, available_extensions)) {
        std::cout << "Physical device " << device << "doesn't support extension " << extension << std::endl;
        return false;
      }
    }

    //Check device properties ahnd features
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;

    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);

    uint32_t major_version = VK_VERSION_MAJOR(device_properties.apiVersion);
    uint32_t minor_version = VK_VERSION_MINOR(device_properties.apiVersion);
    uint32_t patch_version = VK_VERSION_PATCH(device_properties.apiVersion);

    if (major_version < 1 && device_properties.limits.maxImageDimension2D < 4096) {
      std::cout << "Physical device: " << device_properties.deviceName << 
                   "doesn't support required features" << std::endl;
      return 0;
    }

    //Better score to VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
    uint32_t type_score = (device_properties.deviceType ^ 0x5) * 100;
    //Set device score depending on type and max image dimension 
    uint32_t device_score = device_properties.limits.maxImageDimension2D + type_score;

    //Looking for queue families that supports graphics and present operations with at least 3 queues available.
    Queue* new_queue = getQueueFamilyWithProperties(device, (SUPPORT_GRAPHICS_OP | SUPPORT_PRESENT_OP), 3, &window_surface);

    if (!new_queue) {
      std::cout << "Couldn't find any queue family with required operations in device " << 
                    device_properties.deviceName << std::endl;
      return 0;
    }


    return device_score;
  }
}

bool vkdev::Device::createDevice(Instance& instance, Window& window)
{
  if (device_) {
    std::cout << "Device is already created" << std::endl;
    return false;
  }

  uint32_t num_devices = 0;
  if ((vkEnumeratePhysicalDevices(instance.get(), &num_devices, nullptr) != VK_SUCCESS) || (num_devices == 0)) {
    std::cout << "Physical device enumeration failed" << std::endl;
    return false;
  }

  std::vector<VkPhysicalDevice> physical_devices(num_devices);
  if (vkEnumeratePhysicalDevices(instance.get(), &num_devices, physical_devices.data()) != VK_SUCCESS) {
    std::cout << "Physical device enumeration failed" << std::endl;
    return false;
  }

  uint32_t graphics_family_index = UINT32_MAX;
  uint32_t present_queue_family = UINT32_MAX;
  VkPhysicalDevice new_device = VK_NULL_HANDLE;
  
  uint32_t final_score = 0;
  for (auto device : physical_devices) {
    uint32_t gfamily_index;
    uint32_t pfamily_index;
    uint32_t score = checkPhysicalDeviceProperties(device, window, gfamily_index, pfamily_index);
    if (score > final_score) {
      final_score = score;
      graphics_family_index = gfamily_index;
      new_device = device;
    }
  }

  if (new_device == VK_NULL_HANDLE) {
    std::cout << "Couldn't find a physical device based on the chosen properties" << std::endl;
    return false;
  }

  std::vector<float> queue_priority{1.0f};
  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = graphics_family_index;
  queue_create_info.queueCount = queue_priority.size();
  queue_create_info.pQueuePriorities = queue_priority.data();

  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pQueueCreateInfos = &queue_create_info;

  if (vkCreateDevice(new_device, &device_create_info, nullptr, &device_) != VK_SUCCESS) {
    std::cout << "Couldn't create a vulkan device" << std::endl;
    return false;
  }

  if (!LoadDeviceLevelEntryPoints(device_)) {
    return false;
  }
  
  //vkGetDeviceQueue(device_, queueFamily_, 0, &deviceQueue_.queue_);

  return true;
}

