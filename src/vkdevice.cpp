#include "vkdevice.h"
#include "vkfunctions.h"
#include "vkloader.h"
#include <iostream>
#include <vector>

vkdev::Device::Device(VkInstance& instance)
{
  device_ = VK_NULL_HANDLE;
  createDevice(instance);
}

vkdev::Device::~Device()
{
  if (device_ != VK_NULL_HANDLE) {
    vkDeviceWaitIdle(device_);
    vkDestroyDevice(device_, nullptr);
    device_ = VK_NULL_HANDLE;
  }
}

uint32_t vkdev::Device::checkPhysicalDeviceProperties(VkPhysicalDevice& device, uint32_t& queue_family) 
{
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

  uint32_t device_score = device_properties.limits.maxImageDimension2D;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  if (!queue_family_count) {
    std::cout << "Physical device" << device_properties.deviceName << 
                 "doesn't have any queue families!" << std::endl; 
    
    return 0;
  }

  device_score += queue_family_count;

  std::vector<VkQueueFamilyProperties> family_prperties(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, family_prperties.data());
  int32_t family_index = 0;
  for (auto property : family_prperties) {
    if ( (property.queueCount > 0) && 
         (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
           uint32_t type_score = (device_properties.deviceType ^ 0x5);
           device_score += property.queueCount + type_score;
           queue_family = family_index;

           return device_score;
       }
    ++family_index;
  }

  std::cout << "Couldn't find queue family with required properties on physical device " << 
  device_properties.deviceName << std::endl;

  return 0;
}


bool vkdev::Device::createDevice(VkInstance& instance)
{

  uint32_t num_devices = 0;

  if ((vkEnumeratePhysicalDevices(instance, &num_devices, nullptr) != VK_SUCCESS) || (num_devices == 0)) {
    std::cout << "Physical device enumeration failed" << std::endl;
    return false;
  }

  std::vector<VkPhysicalDevice> physical_devices(num_devices);

  if (vkEnumeratePhysicalDevices(instance, &num_devices, physical_devices.data()) != VK_SUCCESS) {
    std::cout << "Physical device enumeration failed" << std::endl;
    return false;
  }

  uint32_t queue_family_index = UINT32_MAX;
  VkPhysicalDevice new_device = VK_NULL_HANDLE;
  
  uint32_t final_score = 0;
  for (auto device : physical_devices) {
    uint32_t family_index;
    uint32_t score = checkPhysicalDeviceProperties(device, family_index);
    if (score > final_score) {
      final_score = score;
      queue_family_index = family_index;
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
  queue_create_info.queueFamilyIndex = queue_family_index;
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

  queueFamily_ = queue_family_index;

  if (!LoadDeviceLevelEntryPoints(device_)) {
    return false;
  }
  
  vkGetDeviceQueue(device_, queueFamily_, 0, &deviceQueue_);

  return true;
}

