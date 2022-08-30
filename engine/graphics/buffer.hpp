#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>
#include "vk_types.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            class Buffer {
            public:
                Buffer(VkDevice _device, VmaAllocator _vma_allocator, VkDeviceSize instanceSize, uint32_t instanceCount, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
                ~Buffer();

                Buffer(const Buffer &) = delete;
                Buffer &operator=(const Buffer &) = delete;

                VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
                void unmap();

                void write_to_buffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
                VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
                VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
                VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

                VkBuffer get_buffer() const { return vk_buffer; }
                void *get_mapped_memory() const { return mapped; }
                uint32_t get_instance_count() const { return instance_count; }
                VkDeviceSize get_instance_size() const { return instance_size; }
                VkDeviceSize get_alignment_size() const { return instance_size; }
                VkMemoryPropertyFlags get_memory_property_flags() const { return memory_property_flags; }
                VkDeviceSize get_buffer_size() const { return buffer_size; }

            private:
                static VkDeviceSize get_alignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

                VkDevice device;
                VmaAllocator vma_allocator;
                void *mapped = nullptr;
                VkBuffer vk_buffer = {};
                VmaAllocation vma_allocation = {};

                VkDeviceSize buffer_size;
                uint32_t instance_count;
                VkDeviceSize instance_size;
                VkDeviceSize alignment_size;
                MemoryFlags memory_property_flags;
            };
        }
    }
}
