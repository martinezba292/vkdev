#include "vkdevice.h"
#include "vkfunctions.h"
#include <iostream>
#include <vector>

vkdev::Device::Device(VkInstance& instance)
{
  createDevice(instance);
}

vkdev::Device::~Device()
{

}

bool vkdev::Device::checkPhysicalDeviceProperties(VkPhysicalDevice& device, uint32_t queue_family) 
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
    return false;
  }

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  if (!queue_family_count) {
    
  }

  return true;
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

  for (auto device : physical_devices) {
    if (checkPhysicalDeviceProperties(device, queue_family_index)) {

    }
  }





  //VkDeviceCreateInfo deviceCreateInfo = {};

  //VkPhysicalDevice physical_dev;
  //vkCreateDevice(physical_dev, &deviceCreateInfo, nullptr, &logical_device_);

  return true;
}

