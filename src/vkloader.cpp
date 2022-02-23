#include <iostream>
#include "vkloader.h"
#include "vkfunctions.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR
  typedef HINSTANCE LibraryHandle;
#elif defined (VK_USE_PLATFORM_XCB_KHR) || defined (VK_USE_PLATFORM_XLIB_KHR)
  #include <dlfcn.h>
  typedef void* LibraryHandle;
#endif

LibraryHandle vklib = nullptr;

bool vkdev::PrepareVulkan()
{
  if (!LoadVulkanLibrary()) 
    return false;

  if (!LoadExportedEntryPoints())
    return false;

  if (!LoadGlobalLevelEntryPoints())
    return false;

  return true;
}

bool vkdev::LoadVulkanLibrary()
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
  vklib = LoadLibrary(TEXT("vulkan-1.dll"));
#elif defined (VK_USE_PLATFORM_XCB_KHR) || defined (VK_USE_PLATFORM_XLIB_KHR)
  vklib = dlopen("libvulkan.so", RTLD_NOW);
#endif

  if (vklib == nullptr) {
    std::cout << "Failed to load Vulkan!!" << std::endl;
    return false;
  } 
  return true;
}


bool vkdev::LoadExportedEntryPoints()
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
#define LoadProcAddress GetProcAddress
#elif defined (VK_USE_PLATFORM_XCB_KHR) || defined (VK_USE_PLATFORM_XLIB_KHR)
#define LoadProcAddress dlsym
#endif

#define VK_EXPORTED_FUNCTION( fun )                                               \
  if ( !(fun = (PFN_##fun)LoadProcAddress(vklib, #fun)) ) {                       \
    std::cout << "Couldn't load exported function:" << #fun << "!" << std::endl;  \
    return false;                                                                 \
  }

#include "functionlist.inl"

  return true;
}


bool vkdev::LoadGlobalLevelEntryPoints()
{
#define VK_GLOBAL_LEVEL_FUNCTION(fun)                                                   \
  if ( !(fun = (PFN_##fun)vkGetInstanceProcAddr(nullptr, #fun) )) {                     \
    std::cout << "Couldn't load a global level function: " << #fun << "!" << std::endl; \
    return false;                                                                       \
  }

#include "functionlist.inl"

  return true;
}


bool vkdev::LoadInstanceLevelEntryPoints(const VkInstance& instance)
{
#define VK_INSTANCE_LEVEL_FUNCTION(fun)                                                    \
  if (!(fun = (PFN_##fun)vkGetInstanceProcAddr(instance, #fun) )) {                        \
    std::cout << "Couldn't load an instance level function: " << #fun << "!" << std::endl; \
    return false;                                                                          \
  }

#include "functionlist.inl"

  return true;
}


bool vkdev::LoadDeviceLevelEntryPoints(const VkDevice& device)
{
#define VK_DEVICE_LEVEL_FUNCTION(fun)                                                    \
  if (!(fun = (PFN_##fun)vkGetDeviceProcAddr(device, #fun) )) {                          \
    std::cout << "Couldn't load a device level function: " << #fun << "!" << std::endl;  \
    return false;                                                                        \
  }

#include "functionlist.inl"

  return true;
}


bool vkdev::CloseVulkan()
{
  if (vklib) {
#ifdef VK_USE_PLATFORM_WIN32_KHR
    FreeLibrary(vklib);
#elif defined (VK_USE_PLATFORM_XCB_KHR) || defined (VK_USE_PLATFORM_XLIB_KHR)
    dlclose(vklib);
#endif
    return true;
  }
  return false;
}

