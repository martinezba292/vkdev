#ifndef __VKTEXTURE_H__
#define __VKTEXTURE_H__ 1

#include "common_def.h"

namespace vkdev {
  class Texture {
    public:
      Texture():textureId_(-1){}
      ~Texture(){}

      bool loadTexture(const char* path);
      const int32_t getId() const;

    private:
      int32_t textureId_;
  };
}

#endif