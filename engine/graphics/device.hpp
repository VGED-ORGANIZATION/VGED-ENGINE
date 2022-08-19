#pragma once

#include "../core/window.hpp"

#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VK_NO_PROTOTYPES
#include <vk_mem_alloc.h>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            class Device {
                public:
                    Device(Window* _window);
                    ~Device();

                    void create_buffer();
                    void create_image();
                    void create_image_view();

                    VkCommandBuffer begin_single_time_command_buffer();
                    void end_single_time_command_buffer(VkCommandBuffer command_buffer) const;

                private:
                    VkInstance vk_instance = {};
                    VkPhysicalDevice vk_physical_device = {};
                    VkDevice vk_device = {};
                    VolkDeviceTable volk_device_table = {};
                    VmaAllocator vma_allocator = {};

                    Window* window;
            };
        }
    }
}