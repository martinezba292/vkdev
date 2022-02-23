#include "internal_resources.h"

std::shared_ptr<vkdev::ResourceManager::InternalResources> vkdev::ResourceManager::Resources_;

// std::shared_ptr<vkdev::ResourceManager::InternalResources> vkdev::ResourceManager::GetResources() {
//   auto instance = Resources_.lock();
//   if (!instance) {
//       instance.reset(new InternalResources());
//       Resources_ = instance;
//   }

//   return instance;
// }