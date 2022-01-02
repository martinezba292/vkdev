  ///////////////////////////////
 //  EXPORTED FUNCTION LIST   //
///////////////////////////////
#ifndef VK_EXPORTED_FUNCTION
#define VK_EXPORTED_FUNCTION( fun )
#endif

VK_EXPORTED_FUNCTION(vkGetInstanceProcAddr)

#undef VK_EXPORTED_FUNCTION


  ///////////////////////////////
 //   GLOBAL FUNCTION LIST    //
///////////////////////////////
#ifndef VK_GLOBAL_LEVEL_FUNCTION
#define VK_GLOBAL_LEVEL_FUNCTION(fun)
#endif

VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance)
VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties)
VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties)

#undef VK_GLOBAL_LEVEL_FUNCTION


  ///////////////////////////////
 //   INSTANCE FUNCTION LIST  //
///////////////////////////////
#ifndef VK_INSTANCE_LEVEL_FUNCTION
#define VK_INSTANCE_LEVEL_FUNCTION(fun)
#endif

VK_INSTANCE_LEVEL_FUNCTION(vkDestroyInstance)
VK_INSTANCE_LEVEL_FUNCTION(vkEnumeratePhysicalDevices)
VK_INSTANCE_LEVEL_FUNCTION(vkEnumerateDeviceExtensionProperties)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceProperties)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFeatures)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
VK_INSTANCE_LEVEL_FUNCTION(vkCreateDevice)
VK_INSTANCE_LEVEL_FUNCTION(vkGetDeviceProcAddr)
//SwapChain functions
VK_INSTANCE_LEVEL_FUNCTION(vkDestroySurfaceKHR)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)
VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)

#ifdef VK_USE_PLATFORM_WIN32_KHR
VK_INSTANCE_LEVEL_FUNCTION(vkCreateWin32SurfaceKHR)
#elif defined VK_USE_PLATFORM_XLIB_KHR
VK_INSTANCE_LEVEL_FUNCTION(vkCreateXlibSurfaceKHR)
#elif defined VK_USE_PLATFORM_XCB_KHR
VK_INSTANCE_LEVEL_FUNCTION(vkCreateXcbSurfaceKHR)
#endif

#undef VK_INSTANCE_LEVEL_FUNCTION


  ///////////////////////////////
 //   DEVICE FUNCTION LIST    //
///////////////////////////////
#ifndef VK_DEVICE_LEVEL_FUNCTION
#define VK_DEVICE_LEVEL_FUNCTION(fun)
#endif

VK_DEVICE_LEVEL_FUNCTION(vkGetDeviceQueue)
VK_DEVICE_LEVEL_FUNCTION(vkDestroyDevice)
VK_DEVICE_LEVEL_FUNCTION(vkDeviceWaitIdle)
VK_DEVICE_LEVEL_FUNCTION(vkCreateSemaphore)

VK_DEVICE_LEVEL_FUNCTION(vkCreateSwapchainKHR)
VK_DEVICE_LEVEL_FUNCTION(vkDestroySwapchainKHR)
VK_DEVICE_LEVEL_FUNCTION(vkGetSwapchainImagesKHR)
VK_DEVICE_LEVEL_FUNCTION(vkAcquireNextImageKHR)
VK_DEVICE_LEVEL_FUNCTION(vkQueueSubmit)
VK_DEVICE_LEVEL_FUNCTION(vkQueuePresentKHR)
VK_DEVICE_LEVEL_FUNCTION(vkBeginCommandBuffer)
VK_DEVICE_LEVEL_FUNCTION(vkEndCommandBuffer)
VK_DEVICE_LEVEL_FUNCTION(vkCmdPipelineBarrier)
VK_DEVICE_LEVEL_FUNCTION(vkCmdClearColorImage)
VK_DEVICE_LEVEL_FUNCTION(vkCreateCommandPool)
VK_DEVICE_LEVEL_FUNCTION(vkAllocateCommandBuffers)
VK_DEVICE_LEVEL_FUNCTION(vkFreeCommandBuffers)
VK_DEVICE_LEVEL_FUNCTION(vkDestroyCommandPool)
VK_DEVICE_LEVEL_FUNCTION(vkDestroySemaphore)


#undef VK_DEVICE_LEVEL_FUNCTION