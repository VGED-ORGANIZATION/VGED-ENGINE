/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "buffer.hpp"

#include <cassert>
#include <cstring>
#include <limits>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            /**
             * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
             *
             * @param instanceSize The size of an instance
             * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
             * minUniformBufferOffsetAlignment)
             *
             * @return VkResult of the buffer mapping call
             */
            VkDeviceSize Buffer::get_alignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
                if (minOffsetAlignment > 0) {
                    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
                }
                return instanceSize;
            }

            Buffer::Buffer(VkDevice _device, VmaAllocator _vma_allocator, const BufferInfo& _info) : device{ _device }, vma_allocator{ _vma_allocator }, info{_info} {
                alignment_size = get_alignment(info.instance_size, info.min_offset_alignment);
                buffer_size = alignment_size * info.instance_count;

                VkBufferUsageFlags usage =
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                    VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT |
                    VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT |
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
                    VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT |
                    VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT |
                    VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT |
                    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
                    VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                    VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

                VkBufferCreateInfo vk_buffer_create_info = {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .size = buffer_size,
                    .usage = usage,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .queueFamilyIndexCount = 0,
                    .pQueueFamilyIndices = nullptr
                };

                VmaAllocationCreateInfo vma_allocation_create_info = {
                    .flags = static_cast<VmaAllocationCreateFlags>(info.memory_flags),
                    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                    .memoryTypeBits = std::numeric_limits<u32>::max(),
                    .pool = nullptr,
                    .pUserData = nullptr,
                    .priority = 0.5f,
                };

                vmaCreateBuffer(vma_allocator, &vk_buffer_create_info, &vma_allocation_create_info, &vk_buffer, &vma_allocation, nullptr);
            }

            Buffer::~Buffer() {
                unmap();
                vmaDestroyBuffer(vma_allocator, vk_buffer, vma_allocation);
            }

            /**
             * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
             *
             * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
             * buffer range.
             * @param offset (Optional) Byte offset from beginning
             *
             * @return VkResult of the buffer mapping call
             */
            VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
                assert(vk_buffer && vma_allocation && "Called map on buffer before create");
                return vmaMapMemory(vma_allocator, vma_allocation, &mapped);
            }

            /**
             * Unmap a mapped memory range
             *
             * @note Does not return a result as vkUnmapMemory can't fail
             */
            void Buffer::unmap() {
                if (mapped) {
                    vmaUnmapMemory(vma_allocator, vma_allocation);
                    mapped = nullptr;
                }
            }

            /**
             * Copies the specified data to the mapped buffer. Default value writes whole buffer range
             *
             * @param data Pointer to the data to copy
             * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
             * range.
             * @param offset (Optional) Byte offset from beginning of mapped region
             *
             */
            void Buffer::write_to_buffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
                assert(mapped && "Cannot copy to unmapped buffer");

                if (size == VK_WHOLE_SIZE) {
                    memcpy(mapped, data, buffer_size);
                } else {
                    char *memOffset = (char *)mapped;
                    memOffset += offset;
                    memcpy(memOffset, data, size);
                }
            }

            /**
             * Flush a memory range of the buffer to make it visible to the device
             *
             * @note Only required for non-coherent memory
             *
             * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
             * complete buffer range.
             * @param offset (Optional) Byte offset from beginning
             *
             * @return VkResult of the flush call
             */
            VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
                return vmaFlushAllocation(vma_allocator, vma_allocation, offset, size);
            }

            /**
             * Invalidate a memory range of the buffer to make it visible to the host
             *
             * @note Only required for non-coherent memory
             *
             * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
             * the complete buffer range.
             * @param offset (Optional) Byte offset from beginning
             *
             * @return VkResult of the invalidate call
             */
            VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
                return vmaInvalidateAllocation(vma_allocator, vma_allocation, offset, size);
            }

            /**
             * Create a buffer info descriptor
             *
             * @param size (Optional) Size of the memory range of the descriptor
             * @param offset (Optional) Byte offset from beginning
             *
             * @return VkDescriptorBufferInfo of specified offset and range
             */
            VkDescriptorBufferInfo Buffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset) {
                return VkDescriptorBufferInfo{
                    vk_buffer,
                    offset,
                    size,
                };
            }
        }
    }
}
