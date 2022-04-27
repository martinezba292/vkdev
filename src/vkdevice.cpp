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
#include <algorithm>
#include "internal_resources.h"


/****************************************************************************************/

vkdev::Device::Device()
{
  auto resources = ResourceManager::GetResources();
  resources->device_ = VK_NULL_HANDLE;
  resources->hardDevice_ = VK_NULL_HANDLE;
  //resources->deviceQueue_ = nullptr;
}

/****************************************************************************************/

vkdev::Device::Device(const Instance& instance, const Window& window)
{
  auto resources = ResourceManager::GetResources();
  resources->device_ = VK_NULL_HANDLE;
  resources->hardDevice_ = VK_NULL_HANDLE;
  //resources->deviceQueue_ = nullptr;
  createDevice(instance, window);
}

/****************************************************************************************/

vkdev::Device::~Device()
{
  destroyDevice();
}


bool vkdev::Device::destroyDevice() {
  auto resources = ResourceManager::GetResources();
    DeviceHandle& device = resources->device_;
    if (device != VK_NULL_HANDLE) {
      vkDeviceWaitIdle(device);
      resources->graphicsQueue_.reset();
      resources->presentQueue_.reset();
      vkDestroyDevice(device, nullptr);
      device = VK_NULL_HANDLE;
      return true;
    }

    return false;
}

/****************************************************************************************/

const VkDevice& vkdev::Device::getDeviceHandle() const {
  return ResourceManager::GetResources()->device_;
}

/****************************************************************************************/

const VkPhysicalDevice& vkdev::Device::getCurrentPhysicalDevice() const {
  return ResourceManager::GetResources()->hardDevice_;
}

/****************************************************************************************/

const vkdev::DeviceQueue* vkdev::Device::getGraphicsQueue() const {
  return ResourceManager::GetResources()->graphicsQueue_.get();
} 

/****************************************************************************************/

const vkdev::DeviceQueue* vkdev::Device::getPresentQueue() const {
  return ResourceManager::GetResources()->presentQueue_.get();
} 

/****************************************************************************************/

std::unique_ptr<vkdev::DeviceQueue> vkdev::Device::findQueueFamilyWithProperties(const VkPhysicalDevice& device, const QueueFamilyFlags support, 
                                                                                         const uint16_t min_queue, const Window* window_handle/* = nullptr*/) {
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
  
  if (window_handle && (support & SUPPORT_PRESENT_OP)) {
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
  DeviceQueue new_queue;
  //new_queue.currentDevice_ = &device;
  new_queue.queueCount_ = min_queue;
  new_queue.queueFamily_ = family_index;

  return std::make_unique<DeviceQueue>(new_queue);
}

/****************************************************************************************/

uint32_t vkdev::Device::ratePhysicalDeviceProperties(const VkPhysicalDevice& device, const Window& window_surface) 
{
  //Get device extension availables
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data()) != VK_SUCCESS) {
    std::cout << "Error enumerating physical device" << device << "extension properties" << std::endl;
    return 0;
  }
  
  //Check for Swapchain extension support
  std::vector<const char*> extensions_needed = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };
  
  for (auto extension : extensions_needed) {
    if (!checkExtensionSupport(extension, available_extensions)) {
      std::cout << "Physical device " << device << "doesn't support extension " << extension << std::endl;
      return 0;
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
  ResourceManager::GetResources()->graphicsQueue_ = findQueueFamilyWithProperties(device, (SUPPORT_GRAPHICS_OP | SUPPORT_PRESENT_OP), 2, &window_surface);
  
  if (!ResourceManager::GetResources()->graphicsQueue_) {
    std::cout << "Couldn't find any queue family with required operations in device " << 
                  device_properties.deviceName << std::endl;
    return device_score >> 1;
  }

  std::cout << "Found device with required properties: " << 
                device_properties.deviceName << std::endl;

  return device_score << 1;
}

/****************************************************************************************/

bool vkdev::Device::createDevice(const Instance& instance, const Window& window)
{
  auto resources = ResourceManager::GetResources();
  DeviceHandle& device = resources->device_;

  if (device) {
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

  VkPhysicalDevice selected_device = VK_NULL_HANDLE;  
  uint32_t final_score = 0;
  for (auto device : physical_devices) {
    uint32_t gfamily_index;
    uint32_t pfamily_index;
    uint32_t score = ratePhysicalDeviceProperties(device, window);
    if (score > final_score) {
      final_score = score;
      selected_device = device;
    }
  }

  if (final_score == 0) {
    std::cout << "Couldn't find a physical device with the required properties" << std::endl;
    return false;
  }

  DeviceQueue* q = resources->graphicsQueue_.get();


  auto& pdevice = ResourceManager::GetResources()->hardDevice_;
  pdevice = selected_device;

  float max_priority = 1.5f;
  std::vector<float> queue_priority(q->queueCount_);
  for (size_t i = 0; i < queue_priority.size(); i++) 
    queue_priority[i] = (max_priority-=0.5f);

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = q->queueFamily_;
  queue_create_info.queueCount = q->queueCount_;
  queue_create_info.pQueuePriorities = &queue_priority[0];

  std::vector<const char*> extensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pQueueCreateInfos = &queue_create_info;
  device_create_info.enabledExtensionCount = extensions.size();
  device_create_info.ppEnabledExtensionNames = &extensions[0];

  if (vkCreateDevice(pdevice, &device_create_info, nullptr, &device) != VK_SUCCESS) {
    std::cout << "Couldn't create a vulkan device" << std::endl;
    return false;
  }

  if (!LoadDeviceLevelEntryPoints(device)) {
    return false;
  }

  DeviceQueue* presentq = ResourceManager::GetResources()->presentQueue_.get();
  presentq->queueCount_ = q->queueCount_;
  presentq->queueFamily_ = q->queueFamily_;
  
  vkGetDeviceQueue(device, q->queueFamily_, 0, &q->queue_);
  vkGetDeviceQueue(device, presentq->queueFamily_, 1, &presentq->queue_);
  
  // VkSemaphoreCreateInfo semaphore_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  // vkCreateSemaphore(device_, &semaphore_info, nullptr, &deviceQueue_->imageAvailableSemaphore_);

  return true;
}

