#pragma once

#include "../core/window.hpp"

#include <memory>
#include <volk.h>
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include <string>
#include <vector>

#include "vk_types.hpp"

#include "gpu_resource_manager.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> present_modes;
            };

            struct QueueFamilyIndices {
                uint32_t graphics_family;
                uint32_t present_family;
                bool graphics_family_has_value = false;
                bool present_family_has_value = false;
                bool is_complete() { return graphics_family_has_value && present_family_has_value; }
            };

            class Device {
            public:
#ifdef NDEBUG
                const bool enable_validation_layers = true;
#else
                const bool enable_validation_layers = true;
#endif

                Device(Window &_window);
                ~Device();

                // Not copyable or movable
                Device(const Device &) = delete;
                Device &operator=(const Device &) = delete;
                Device(Device &&) = delete;
                Device &operator=(Device &&) = delete;

                VkCommandPool command_pool() { return vk_command_pool; }
                VkDevice device() { return vk_device; }
                VkSurfaceKHR surface() { return vk_surface_khr; }
                VkQueue graphics_queue() { return vk_graphics_queue; }
                VkQueue present_queue() { return vk_present_queue; }
                VkInstance instance() { return vk_instance; }
                VkPhysicalDevice physical_device() { return vk_physical_device; }
                uint32_t graphics_queue_family() { return find_physical_queue_families().graphics_family; }
                VmaAllocator &allocator() { return vma_allocator; }

                SwapChainSupportDetails get_swap_chain_support() { return query_swap_chain_support(vk_physical_device); }
                uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
                QueueFamilyIndices find_physical_queue_families() { return find_queue_families(vk_physical_device); }
                VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

                // Buffer Helper Functions
                VkCommandBuffer begin_single_time_commands();
                void end_single_time_commands(VkCommandBuffer commandBuffer);
                void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
                void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

                VkPhysicalDeviceProperties properties;

                // std::pair<std::unique_ptr<Buffer>&, u32> create_buffer(const BufferInfo& info);
                std::pair<std::unique_ptr<Image> &, u32> create_image(const ImageInfo &info);
                std::pair<std::unique_ptr<Sampler> &, u32> create_sampler(const SamplerInfo &info);

                void destroy_image(u32 image_id);
                void destroy_sampler(u32 sampler_id);

                std::unique_ptr<Image> &get_image(u32 index);
                std::unique_ptr<Sampler> &get_sampler(u32 index);

            private:
                void create_instance();
                void setup_debug_messenger();
                void create_surface();
                void pick_physical_device();
                void create_logical_device();
                void create_vma_allocator();
                void create_command_pool();

                // helper functions
                bool is_device_suitable(VkPhysicalDevice device);
                std::vector<const char *> get_required_extensions();
                bool check_validation_layer_support();
                QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
                void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
                void has_gflw_required_instance_extensions();
                bool check_device_extension_support(VkPhysicalDevice device);
                SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

                VkInstance vk_instance = {};
                VkDebugUtilsMessengerEXT vk_debug_utils_messenger_ext = {};
                VkPhysicalDevice vk_physical_device = {};
                Window &window;
                VkCommandPool vk_command_pool = {};

                GPUResourceManager *gpu_resource_manager;

                VkDevice vk_device = {};
                VkSurfaceKHR vk_surface_khr = {};
                VkQueue vk_graphics_queue = {};
                VkQueue vk_present_queue = {};
                // VolkDeviceTable volk_device_table = {};
                VmaAllocator vma_allocator = {};

                const std::vector<const char *> validation_layers = { "VK_LAYER_KHRONOS_validation" };
                const std::vector<const char *> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
            };
        }
    }
}
