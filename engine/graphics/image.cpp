#include "image.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            ImageView::ImageView(VkDevice _device, const ImageViewInfo &_info) : device{ _device }, info{ _info } {
                VkImageViewCreateInfo vk_image_view_create_info = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .image = info.image,
                    .viewType = static_cast<VkImageViewType>(info.type),
                    .format = static_cast<VkFormat>(info.format),
                    .components = VkComponentMapping{
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                    },
                    .subresourceRange = *reinterpret_cast<const VkImageSubresourceRange *>(&info.slice)
                };

                vkCreateImageView(device, &vk_image_view_create_info, nullptr, &vk_image_view);
            }

            ImageView::~ImageView() {
                vkDestroyImageView(device, vk_image_view, nullptr);
            }

            Image::Image(VkDevice _device, VmaAllocator _vma_allocator, const ImageInfo &_info) : device{ _device }, vma_allocator{ _vma_allocator }, info{ _info } {
                VkImageCreateInfo vk_image_create_info = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .imageType = static_cast<VkImageType>(info.type),
                    .format = static_cast<VkFormat>(info.format),
                    .extent = *reinterpret_cast<const VkExtent3D *>(&info.size),
                    .mipLevels = info.mip_level_count,
                    .arrayLayers = info.array_layer_count,
                    .samples = static_cast<VkSampleCountFlagBits>(info.sample_count),
                    .tiling = VK_IMAGE_TILING_OPTIMAL,
                    .usage = info.usage,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .queueFamilyIndexCount = 0,
                    .pQueueFamilyIndices = 0,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
                };

                VmaAllocationCreateInfo vma_allocation_create_info{
                    .flags = static_cast<VmaAllocationCreateFlags>(info.memory_flags),
                    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                    .memoryTypeBits = std::numeric_limits<u32>::max(),
                    .pool = nullptr,
                    .pUserData = nullptr,
                    .priority = 0.5f,
                };

                vmaCreateImage(vma_allocator, &vk_image_create_info, &vma_allocation_create_info, &vk_image, &vma_allocation, nullptr);

                ImageViewType type = static_cast<ImageViewType>(info.type);
                if (info.array_layer_count > 1) {
                    type = static_cast<ImageViewType>(static_cast<u32>(info.type) + 3);
                }

                ImageViewInfo image_view_info = {
                    .type = type,
                    .format = info.format,
                    .image = vk_image,
                    .slice = {
                        .image_aspect = info.aspect,
                        .base_mip_level = 0,
                        .level_count = info.mip_level_count,
                        .base_array_layer = 0,
                        .layer_count = info.array_layer_count,
                    },
                };

                image_view_ = new ImageView(device, image_view_info);
            }

            Image::~Image() {
                delete image_view_;

                vmaDestroyImage(vma_allocator, vk_image, vma_allocation);
            }

            Sampler::Sampler(VkDevice _device, const SamplerInfo &_info) : device{ _device }, info{ _info } {
                VkSamplerCreateInfo vk_sampler_create_info = {
                    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .magFilter = static_cast<VkFilter>(info.magnification_filter),
                    .minFilter = static_cast<VkFilter>(info.minification_filter),
                    .mipmapMode = static_cast<VkSamplerMipmapMode>(info.mipmap_filter),
                    .addressModeU = static_cast<VkSamplerAddressMode>(info.address_mode_u),
                    .addressModeV = static_cast<VkSamplerAddressMode>(info.address_mode_v),
                    .addressModeW = static_cast<VkSamplerAddressMode>(info.address_mode_w),
                    .mipLodBias = info.mip_lod_bias,
                    .anisotropyEnable = info.enable_anisotropy,
                    .maxAnisotropy = info.max_anisotropy,
                    .compareEnable = info.enable_compare,
                    .compareOp = static_cast<VkCompareOp>(info.compareOp),
                    .minLod = info.min_lod,
                    .maxLod = info.max_lod,
                    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                    .unnormalizedCoordinates = info.enable_unnormalized_coordinates,
                };

                vkCreateSampler(device, &vk_sampler_create_info, nullptr, &vk_sampler);
            }
            Sampler::~Sampler() {
                vkDestroySampler(device, vk_sampler, nullptr);
            }
        }
    }
}