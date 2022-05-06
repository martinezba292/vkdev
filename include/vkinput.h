#ifndef __VK_INPUT_MANAGER_H__
#define __VK_INPUT_MANAGER_H__ 1

#include "vector2.hpp"

namespace vkdev {
  enum InputKeys {
    InputKeys_ESC = 0,
    InputKeys_W = 1,
    InputKeys_A,
    InputKeys_S,
    InputKeys_D,
    InputKeys_MAX
  };
  class Window;
  class InputManager {
    public:
      InputManager();
      ~InputManager();
      
      void update(const Window& w);
      bool getKey(const InputKeys key) const;
      const lau::Vector2D& getMousePosition() const;
      bool getMouseClick() const;

    private:  
      void reset();


    private:
      lau::Vector2D mousePosition_;
      bool input_[InputKeys_MAX];
      bool enable_;
      bool clicked_;
  };
}

#endif