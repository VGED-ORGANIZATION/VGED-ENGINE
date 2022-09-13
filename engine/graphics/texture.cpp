#include "texture.hpp"
#include <stdexcept>
#include "device.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include "graphics/vk_types.hpp"
#include "stb_image.h"
#include <cmath>
#include "buffer.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            Texture::Texture(Device &_device, const std::string &filepath) : device{ _device } {
                int channels;
                int m_BytesPerPixel;

                auto data = stbi_load(filepath.c_str(), &width, &height, &m_BytesPerPixel, 4);

                mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

                //Buffer stagingBuffer{ device.device(), device.allocator(), 4, static_cast<uint32_t>(width * height), MemoryFlagBits::HOST_ACCESS_SEQUENTIAL_WRITE };

                Buffer stagingBuffer{ device.device(), device.allocator(), BufferInfo {
                    .instance_size = sizeof(u8) * 4,
                    .instance_count = static_cast<uint32_t>(width * height),
                    .memory_flags = MemoryFlagBits::HOST_ACCESS_SEQUENTIAL_WRITE
                }};

                stagingBuffer.map();
                stagingBuffer.write_to_buffer(data);

                imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

                auto [image, img_id] = device.create_image({ .format = static_cast<ImageFormat>(imageFormat),
                                                             .size = { width, height, 1 },
                                                             .mip_level_count = static_cast<u32>(mipLevels),
                                                             .usage = ImageUsageFlagBits::TRANSFER_SRC | ImageUsageFlagBits::TRANSFER_DST | ImageUsageFlagBits::SAMPLED,
                                                             .memory_flags = MemoryFlagBits::DEDICATED_MEMORY });

                image_id = img_id;

                transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                device.copy_buffer_to_image(stagingBuffer.get_buffer(), image->image(), static_cast<uint>(width), static_cast<uint>(height), 1);

                generateMipmaps();
                imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                auto [sampler, sampler_id_] = device.create_sampler({
                    .address_mode_u = SamplerAddressMode::REPEAT,
                    .address_mode_v = SamplerAddressMode::REPEAT,
                    .address_mode_w = SamplerAddressMode::REPEAT,
                    .mip_lod_bias = 0.0f,
                    .max_anisotropy = 4.0f,
                    .max_lod = static_cast<float>(mipLevels),
                });

                sampler_id = sampler_id_;

                stbi_image_free(data);
            }

            Texture::~Texture() {
                device.destroy_image(image_id);
                device.destroy_sampler(sampler_id);
            }

            void Texture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
                VkCommandBuffer commandBuffer = device.begin_single_time_commands();

                auto vk_image = device.get_image(image_id)->image();

                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.oldLayout = oldLayout;
                barrier.newLayout = newLayout;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = vk_image;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = mipLevels;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;

                VkPipelineStageFlags sourceStage;
                VkPipelineStageFlags destinationStage;

                if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                    barrier.srcAccessMask = 0;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                } else {
                    throw std::runtime_error("unsupported layout transition!");
                }

                vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                device.end_single_time_commands(commandBuffer);
            }

            void Texture::generateMipmaps() {
                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(device.physical_device(), imageFormat, &formatProperties);

                if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                    throw std::runtime_error("texture image format does not support linear blitting!");
                }

                VkCommandBuffer commandBuffer = device.begin_single_time_commands();

                auto vk_image = device.get_image(image_id)->image();

                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.image = vk_image;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = 1;
                barrier.subresourceRange.levelCount = 1;

                int32_t mipWidth = width;
                int32_t mipHeight = height;

                for (uint32_t i = 1; i < mipLevels; i++) {
                    barrier.subresourceRange.baseMipLevel = i - 1;
                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                    VkImageBlit blit{};
                    blit.srcOffsets[0] = { 0, 0, 0 };
                    blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.srcSubresource.mipLevel = i - 1;
                    blit.srcSubresource.baseArrayLayer = 0;
                    blit.srcSubresource.layerCount = 1;
                    blit.dstOffsets[0] = { 0, 0, 0 };
                    blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    blit.dstSubresource.mipLevel = i;
                    blit.dstSubresource.baseArrayLayer = 0;
                    blit.dstSubresource.layerCount = 1;

                    vkCmdBlitImage(commandBuffer, vk_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                    if (mipWidth > 1)
                        mipWidth /= 2;
                    if (mipHeight > 1)
                        mipHeight /= 2;
                }

                barrier.subresourceRange.baseMipLevel = mipLevels - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                device.end_single_time_commands(commandBuffer);
            }
        }
    }
}