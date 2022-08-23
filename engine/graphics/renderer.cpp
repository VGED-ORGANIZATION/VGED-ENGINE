#include "renderer.hpp"
#include "../core/window.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			Renderer::Renderer(Window &_window, Device &_device) : window{ _window }, device{ _device } {
				recreateSwapChain();
				createCommandBuffers();
			}

			Renderer::~Renderer() { freeCommandBuffers(); }

			void Renderer::recreateSwapChain() {
				auto extent = window.get_extent();
				while (extent.width == 0 || extent.height == 0) {
					extent = window.get_extent();
					glfwWaitEvents();
				}
				vkDeviceWaitIdle(device.device());

				if (swap_chain == nullptr) {
					swap_chain = std::make_unique<SwapChain>(device, extent);
				} else {
					std::shared_ptr<SwapChain> oldSwapChain = std::move(swap_chain);
					swap_chain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

					if (!oldSwapChain->compareSwapFormats(*swap_chain.get())) {
						throw std::runtime_error("Swap chain image(or depth) format has changed!");
					}
				}
			}

			void Renderer::createCommandBuffers() {
				commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = device.command_pool();
				allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

				if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate command buffers!");
				}
			}

			void Renderer::freeCommandBuffers() {
				vkFreeCommandBuffers(device.device(), device.command_pool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
				commandBuffers.clear();
			}

			VkCommandBuffer Renderer::beginFrame() {
				assert(!isFrameStarted && "Can't call beginFrame while already in progress");

				auto result = swap_chain->acquireNextImage(&currentImageIndex);
				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					recreateSwapChain();
					return nullptr;
				}

				if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
					throw std::runtime_error("failed to acquire swap chain image!");
				}

				isFrameStarted = true;

				auto commandBuffer = getCurrentCommandBuffer();
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

				if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}
				return commandBuffer;
			}

			void Renderer::endFrame() {
				assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
				auto commandBuffer = getCurrentCommandBuffer();
				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}

				auto result = swap_chain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
				if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.was_resized()) {
					window.reset_resized_flag();
					recreateSwapChain();
				} else if (result != VK_SUCCESS) {
					throw std::runtime_error("failed to present swap chain image!");
				}

				isFrameStarted = false;
				currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
			}

			void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
				assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
				assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = swap_chain->getRenderPass();
				renderPassInfo.framebuffer = swap_chain->getFrameBuffer(currentImageIndex);

				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swap_chain->getSwapChainExtent();

				std::array<VkClearValue, 2> clearValues{};
				clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
				clearValues[1].depthStencil = { 1.0f, 0 };
				renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();

				vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = static_cast<float>(swap_chain->getSwapChainExtent().width);
				viewport.height = static_cast<float>(swap_chain->getSwapChainExtent().height);
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				VkRect2D scissor{ { 0, 0 }, swap_chain->getSwapChainExtent() };
				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
			}

			void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
				assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
				assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
				vkCmdEndRenderPass(commandBuffer);
			}
		}
	}
}
