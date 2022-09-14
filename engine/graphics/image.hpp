#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>
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
                ImageView(VkDevice _device, const ImageViewInfo &_info);
                ~ImageView();

                VkImageView &image_view() { return vk_image_view; }

            private:
                VkDevice device;

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
                Image(VkDevice _device, VmaAllocator _vma_allocator, const ImageInfo &_info);
                ~Image();

                VkImageView &image_view() { return image_view_->image_view(); }
                VkImage &image() { return vk_image; }

            private:
                VkDevice device;
                VmaAllocator vma_allocator;

                ImageView *image_view_ = {};

                VkImage vk_image = {};
                VmaAllocation vma_allocation = {};
                ImageInfo info = {};
            };

            struct SamplerInfo {
                Filter magnification_filter = Filter::LINEAR;
                Filter minification_filter = Filter::LINEAR;
                Filter mipmap_filter = Filter::LINEAR;
                SamplerAddressMode address_mode_u = SamplerAddressMode::CLAMP_TO_EDGE;
                SamplerAddressMode address_mode_v = SamplerAddressMode::CLAMP_TO_EDGE;
                SamplerAddressMode address_mode_w = SamplerAddressMode::CLAMP_TO_EDGE;
                f32 mip_lod_bias = 0.5f;
                bool enable_anisotropy = false;
                f32 max_anisotropy = 0.0f;
                bool enable_compare = false;
                CompareOp compareOp = CompareOp::ALWAYS;
                f32 min_lod = 0.0f;
                f32 max_lod = 1.0f;
                bool enable_unnormalized_coordinates = false;
            };

            class Sampler {
            public:
                Sampler(VkDevice _device, const SamplerInfo &_info);
                ~Sampler();

                VkSampler &sampler() { return vk_sampler; }

            private:
                VkDevice device;
                VkSampler vk_sampler = {};
                SamplerInfo info = {};
            };
        }
    }
}