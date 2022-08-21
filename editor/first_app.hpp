#pragma once

#include "../engine/graphics/lve_device.hpp"
#include "../engine/graphics/lve_model.hpp"
#include "../engine/graphics/lve_pipeline.hpp"
#include "../engine/graphics/lve_swap_chain.hpp"
#include "../engine/core/lve_window.hpp"
#include "../engine/graphics/lve_imgui.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
class FirstApp {
 public:
  static constexpr int WIDTH = 1280;
  static constexpr int HEIGHT = 720;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

 private:
  void loadModels();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);

  LveWindow lveWindow{WIDTH, HEIGHT, "VGED Engine"};
  LveDevice lveDevice{lveWindow};
  std::unique_ptr<LveSwapChain> lveSwapChain;
  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<LveModel> lveModel;
};
}  // namespace lve
