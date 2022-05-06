#include "vkinput.h"
#include "common_def.h"
#include "glfw3.h"
#include <array>
#include "window.h"

vkdev::InputManager::InputManager() {
  mousePosition_ = {WINDOW_W * 0.5f, WINDOW_H * 0.5f};
  enable_ = false;
  reset();
}

vkdev::InputManager::~InputManager() {
    
}


void vkdev::InputManager::update(const Window& w) {
  glfwPollEvents();
  GLFWwindow* window = const_cast<GLFWwindow*>(w.getWindow());
  input_[InputKeys_ESC] = glfwGetKey(window, GLFW_KEY_ESCAPE);

  bool detect = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (!detect) {
    if (!enable_) return;
    
    enable_ = detect;
    reset();    
    return;
  }


  double x, y;
  glfwGetCursorPos(window, &x, &y);
  mousePosition_ = {static_cast<float>(x), static_cast<float>(y)};

  clicked_ = (detect && !enable_);
  enable_ = detect;
  std::array<int32_t, InputKeys_MAX> glfwkeys = {
    GLFW_KEY_W,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D
  };

  for (size_t i = 1; i < InputKeys_MAX; i++) {
    input_[i] = glfwGetKey(window, glfwkeys[i-1]);
  }
}

void vkdev::InputManager::reset() {
  for (size_t i = 0; i < InputKeys_MAX; i++) {
    input_[i] = false;
  } 
}


bool vkdev::InputManager::getKey(const InputKeys key) const {
  return input_[key];
}

const lau::Vector2D& vkdev::InputManager::getMousePosition() const {
  return mousePosition_;
}

bool vkdev::InputManager::getMouseClick() const {
  return clicked_;
}