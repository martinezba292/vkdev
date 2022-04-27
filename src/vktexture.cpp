#include "vktexture.h"
#include "internal_resources.h"
#include <iostream>

bool vkdev::Texture::loadTexture(const char* path) {
  if (!path) {
    std::cout << "Invalid texture path" << std::endl;
    return false;
  }

  textureId_ = ResourceManager::GetResources()->textures_.size();
  TextureData texture {
      path,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE,
      VK_NULL_HANDLE
  };

  ResourceManager::GetResources()->textures_.push_back(std::move(texture));
  
  return true;
}

const int32_t vkdev::Texture::getId() const {
  return textureId_;
}

