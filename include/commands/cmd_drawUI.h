#ifndef __CMD_DRAWUI_H__
#define __CMD_DRAWUI_H__ 1

#include "vkcommand.h"
#include "UI.h"
#include "imgui.h"
#include "vkfunctions.h"
#include "backends/imgui_impl_vulkan.h"

namespace vkdev {
  //class UI;
  class DrawUICmd : public Command {
    public:
      DrawUICmd(const UI* ui) : interface_(ui) { type_ = CommandType_DrawUI; }
      ~DrawUICmd() {}

      inline void record(const VkCommandBuffer& cmd) const override {
        interface_->render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
      }

    private:
      const UI* interface_;
  };
}

#endif