#pragma once

#include "device.hpp"
#include <string.h>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			class Texture {
			public:
				Texture(Device &_device, const std::string &filepath);
				~Texture();

				Texture(const Texture &) = delete;
				Texture &operator=(const Texture &) = delete;
				Texture(Texture &&) = delete;
				Texture &operator=(Texture &&) = delete;

				VkSampler getSampler() { return sampler; }
				VkImageView getImageView() { return imageView; }
				VkImageLayout getImageLayout() { return imageLayout; }

			private:
				void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
				void generateMipmaps();

				int width, height, mipLevels;

				Device &device;
				VkImage image;
				VkDeviceMemory imageMemory;
				VkImageView imageView;
				VkSampler sampler;
				VkFormat imageFormat;
				VkImageLayout imageLayout;
			};
		}
	}
}