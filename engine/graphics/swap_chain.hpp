#pragma once

#include "device.hpp"
#include "image.hpp"

#include <memory>
#include <string>
#include <vector>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			class SwapChain {
			public:
				static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

				SwapChain(Device &_device, VkExtent2D windowExtent);
				SwapChain(Device &_device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);

				~SwapChain();

				SwapChain(const SwapChain &) = delete;
				SwapChain &operator=(const SwapChain &) = delete;

				VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
				VkRenderPass getRenderPass() { return renderPass; }
				VkImageView getImageView(int index) { return swapChainImageViews[index]->image_view(); }
				size_t imageCount() { return swapChainImages.size(); }
				VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
				VkExtent2D getSwapChainExtent() { return swapChainExtent; }
				uint32_t width() { return swapChainExtent.width; }
				uint32_t height() { return swapChainExtent.height; }

				float extentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }
				VkFormat findDepthFormat();

				VkResult acquireNextImage(uint32_t *imageIndex);
				VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

				bool compareSwapFormats(const SwapChain &swapChain) const { return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat; }

			private:
				void init();
				void createSwapChain();
				void createImageViews();
				void createDepthResources();
				void createRenderPass();
				void createFramebuffers();
				void createSyncObjects();

				// Helper functions
				VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
				VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
				VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

				VkFormat swapChainImageFormat;
				VkFormat swapChainDepthFormat;
				VkExtent2D swapChainExtent;

				std::vector<VkFramebuffer> swapChainFramebuffers;
				VkRenderPass renderPass;

				std::vector<std::unique_ptr<Image>> depthImages;
				std::vector<VkImage> swapChainImages;
				std::vector<std::unique_ptr<ImageView>> swapChainImageViews;

				Device &device;
				VkExtent2D windowExtent;

				VkSwapchainKHR swapChain;
				std::shared_ptr<SwapChain> oldSwapChain;

				std::vector<VkSemaphore> imageAvailableSemaphores;
				std::vector<VkSemaphore> renderFinishedSemaphores;
				std::vector<VkFence> inFlightFences;
				std::vector<VkFence> imagesInFlight;
				size_t currentFrame = 0;
			};
		}
	}
}
