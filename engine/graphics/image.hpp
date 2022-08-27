#pragma once

#include "device.hpp"
#include "vk_types.hpp"

#include <glm/glm.hpp>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			struct ImageViewInfo {
				ImageViewType type = ImageViewType::TYPE_2D;
				ImageFormat format = ImageFormat::R8G8B8A8_UNORM;
				VkImage image = {};
				ImageMipArraySlice slice = {};
			};

			class ImageView {
			public:
                ImageView(Device& _device, const ImageViewInfo& _info);
                ~ImageView();

                VkImageView& image_view() { return vk_image_view; }
			private:
                Device& device;

                VkImageView vk_image_view = {};
                ImageViewInfo info = {};
			};

			struct ImageInfo {
                ImageType type = ImageType::TYPE_2D;
				ImageFormat format = ImageFormat::R8G8B8A8_UNORM;
				ImageAspectFlags aspect = ImageAspectFlagBits::COLOR;
				glm::ivec3 size = { 1, 1, 1 };
                u32 mip_level_count = 1;
				u32 array_layer_count = 1;
				u32 sample_count = 1;
				ImageUsageFlags usage = {};
				MemoryFlags memory_flags = {};
			};

			class Image {
			public:
                Image(Device& _device, const ImageInfo& _info);
                ~Image();

                VkImageView& image_view() { return image_view_->image_view(); }
                VkImage& image() { return vk_image; }
			private:
                Device& device;

                ImageView* image_view_ = {};

                VkImage vk_image = {};
                VmaAllocation vma_allocation = {};
                ImageInfo info = {};
			};
		}
	}
}