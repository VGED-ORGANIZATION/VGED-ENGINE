#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "../core/window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			class Renderer {
			public:
				Renderer(Window &_window, Device &_device);
				~Renderer();

				Renderer(const Renderer &) = delete;
				Renderer &operator=(const Renderer &) = delete;

				VkRenderPass getSwapChainRenderPass() const { return swap_chain->getRenderPass(); }
				float getAspectRatio() const { return swap_chain->extentAspectRatio(); }
				bool isFrameInProgress() const { return isFrameStarted; }
				uint32_t getImageCount() const { return swap_chain->imageCount(); }

				VkCommandBuffer getCurrentCommandBuffer() const {
					assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
					return commandBuffers[currentFrameIndex];
				}

				int getFrameIndex() const {
					assert(isFrameStarted && "Cannot get frame index when frame not in progress");
					return currentFrameIndex;
				}

				VkCommandBuffer beginFrame();
				void endFrame();
				void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
				void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

			private:
				void createCommandBuffers();
				void freeCommandBuffers();
				void recreateSwapChain();

				Window &window;
				Device &device;
				std::unique_ptr<SwapChain> swap_chain;
				std::vector<VkCommandBuffer> commandBuffers;

				uint32_t currentImageIndex;
				int currentFrameIndex{ 0 };
				bool isFrameStarted{ false };
			};
		}
	}
}
