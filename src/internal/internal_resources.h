#ifndef __INTERNAL_RESOURCES_H__
#define __INTERNAL_RESOURCES_H__ 1

#include <memory>
#include <vector>


namespace vkdev {
  class ResourceManager {
      struct InternalResources {
        std::vector<std::vector<float>> vertexData_;
      };
      public:
        static inline std::shared_ptr<InternalResources> GetResources() {
          auto instance = Resources_;
          if (!instance) {
              instance.reset(new InternalResources());
              instance->vertexData_.clear();
              Resources_ = instance;
          }
  
          return instance;
        }

      private:
        static std::shared_ptr<InternalResources> Resources_;
  };
}

#endif