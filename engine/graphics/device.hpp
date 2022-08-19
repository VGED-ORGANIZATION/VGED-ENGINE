#pragma once

#include "../core/window.hpp"
#include "vk_types.hpp"

#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VK_NO_PROTOTYPES
#include <vk_mem_alloc.h>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> present_modes;
            };

            struct QueueFamilyIndices {
                u32 graphics_family{};
                u32 present_family{};
                bool graphics_family_has_value = false;
                bool present_family_has_value = false;
                bool is_complete() const { return graphics_family_has_value && present_family_has_value; }
            };

            class Device {
            public:
                #ifdef NDEBUG
                        const bool enable_validation_layers = true;
                #else
                        const bool enable_validation_layers = true;
                #endif

                explicit Device(Window* _window);
                ~Device();

                Device(const Device &) = delete;
                Device &operator=(const Device &) = delete;
                Device(Device &&) = delete;
                Device &operator=(Device &&) = delete;

                u32 get_graphics_queue_family() { return find_physical_queue_families().graphics_family; }

                SwapChainSupportDetails get_swapchain_support() { return query_swapchain_support(vk_physical_device); }
                u32 find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties) const;
                QueueFamilyIndices find_physical_queue_families() { return find_queue_families(vk_physical_device); }
                VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

                void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, MemoryFlags memory_flags, VkBuffer &vk_buffer, VmaAllocation &vma_allocation);
                void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
                void copy_buffer_to_image(VkBuffer buffer, VkImage image, u32 width, u32 height, u32 layer_count);

                void create_image_with_info(const VkImageCreateInfo &vk_image_create_info, MemoryFlags memory_flags, VkImage &vk_image, VmaAllocation &vma_allocation);

                VkCommandBuffer begin_single_time_command_buffer();
                void end_single_time_command_buffer(VkCommandBuffer command_buffer) const;

                VkCommandPool get_command_pool() { return vk_command_pool; }
                VkDevice device() { return vk_device; }
                VkSurfaceKHR surface() { return vk_surface_khr; }
                VkQueue graphicsQueue() { return vk_graphics_queue; }
                VkQueue presentQueue() { return vk_present_queue; }
                VkInstance get_instance() { return vk_instance; }

            private:
                void create_instance();
                void create_surface();
                void pick_physical_device();
                void create_logical_device();
                void create_vma_allocator();
                void create_command_pool();

                VkDevice vk_device = {};
                VkSurfaceKHR vk_surface_khr = {};
                VkQueue vk_graphics_queue = {};
                VkQueue vk_present_queue = {};
                VkPhysicalDevice vk_physical_device = {};
                VkCommandPool vk_command_pool = {};
                VkInstance vk_instance = {};
                VmaAllocator vma_allocator = {};
                VkPhysicalDeviceProperties properties = {};

                // helper functions
                bool is_device_suitable(VkPhysicalDevice device);
                std::vector<const char *> get_required_extensions() const;
                bool check_validation_layer_support();
                QueueFamilyIndices find_queue_families(VkPhysicalDevice device) const;
                bool check_device_extension_support(VkPhysicalDevice device);
                SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device) const;

                VkDebugUtilsMessengerEXT vk_debug_messenger;
                Window* window;

                VolkDeviceTable volk_device_table = {};

                const std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};
                const std::vector<const char *> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
            };
        }
    }
}