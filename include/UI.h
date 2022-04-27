#ifndef __VK_UI_H__
#define __VK_UI_H__ 1

#include "common_def.h"

namespace vkdev {
  class Instance;
  class Window;
  class RenderPass;
  class CommandBuffer;
  class UI {
    public:
      UI();
      ~UI();

      bool initUI(const Instance& i, const Window& w, 
                  const RenderPass& rp, const CommandBuffer& cmdbuffer,
                  const uint32_t& images);
      void destroyUI();

      void updateFps(float fps);
      void render() const;

    private:
      virtual void drawUI() const;

    private:
      VkDescriptorPool imguiPool_;
      float fpsCounter_;

  };

}

#endif