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
                /**
                 * @brief Construct a new Device object
                 * 
                 * @param _window 
                 */
                explicit Device(Window* _window);
                ~Device();

                Device(const Device &) = delete;
                Device &operator=(const Device &) = delete;
                Device(Device &&) = delete;
                Device &operator=(Device &&) = delete;

                /**
                 * @brief Gets the physical queue family of the selected physical device
                 * 
                 * @return u32 
                 */
                u32 get_graphics_queue_family() { return find_physical_queue_families().graphics_family; }

                /**
                 * @brief Get the swapchain support of the seleted physical device
                 * 
                 * @return SwapChainSupportDetails 
                 */
                SwapChainSupportDetails get_swapchain_support() { return query_swapchain_support(vk_physical_device); }
                
                /**
                 * @brief Get the index of memory of type of the requested property 
                 * 
                 * @param type_filter 
                 * @param properties requested property
                 * @return u32 
                 */
                u32 find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties) const;
                
                /**
                 * @brief Get the queue family support indices for the selected physical device
                 * 
                 * @return QueueFamilyIndices 
                 */
                QueueFamilyIndices find_physical_queue_families() { return find_queue_families(vk_physical_device); }

                /**
                 * @brief Gets the supported format from a vector of candidate formats
                 * 
                 * @param candidates 
                 * @param tiling 
                 * @param features 
                 * @return VkFormat 
                 */
                VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;


                /**
                 * @brief Utility function to create VkBuffer
                 * 
                 * @param size total size of the buffer
                 * @param usage buffer useage 
                 * @param memory_flags buffer property flags 
                 * @param vk_buffer the buffer handle that will point to the allocated memory
                 * @param vma_allocation allocation on the gpu 
                 */
                void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, MemoryFlags memory_flags, VkBuffer &vk_buffer, VmaAllocation &vma_allocation);

                /**
                 * @brief Utility function for copying VkBuffer
                 * 
                 * @param src_buffer source buffer 
                 * @param dst_buffer destination buffer
                 * @param size size of the buffers
                 */
                void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

                /**
                 * @brief Utility function for copying a buffer to a VkImage
                 * 
                 * @param buffer source buffer
                 * @param image destination image
                 * @param width width of the image
                 * @param height height of the image 
                 * @param layer_count 
                 */
                void copy_buffer_to_image(VkBuffer buffer, VkImage image, u32 width, u32 height, u32 layer_count);

                /**
                 * @brief Utility function for creating a VkImage
                 * 
                 * @param vk_image_create_info create info struct 
                 * @param memory_flags memory property flags
                 * @param vk_image input image
                 * @param vma_allocation 
                 */
                void create_image_with_info(const VkImageCreateInfo &vk_image_create_info, MemoryFlags memory_flags, VkImage &vk_image, VmaAllocation &vma_allocation);

                /**
                 * @brief Utility function for begining a single use command buffer
                 * 
                 * @return VkCommandBuffer 
                 */
                VkCommandBuffer begin_single_time_command_buffer();

                /**
                 * @brief Utility function for ending a single use command buffer and submitting it to the graphics queue
                 * 
                 * @param command_buffer same command buffer that was given from the begin_single_time_command_buffer()
                 */
                void end_single_time_command_buffer(VkCommandBuffer command_buffer) const;

                /**
                 * @brief Get the command pool object
                 * 
                 * @return VkCommandPool 
                 */
                VkCommandPool get_command_pool() { return vk_command_pool; }

                /**
                 * @brief Get the device handle
                 * 
                 * @return VkDevice 
                 */
                VkDevice device() { return vk_device; }

                /**
                 * @brief Get the surface handle
                 * 
                 * @return VkSurfaceKHR 
                 */
                VkSurfaceKHR surface() { return vk_surface_khr; }
                
                /**
                 * @brief Get the graphics queue handle
                 * 
                 * @return VkQueue 
                 */
                VkQueue graphicsQueue() { return vk_graphics_queue; }
                
                /**
                 * @brief Get the present queue handle
                 * 
                 * @return VkQueue 
                 */
                VkQueue presentQueue() { return vk_present_queue; }

                /**
                 * @brief Get the instance handle
                 * 
                 * @return VkInstance 
                 */
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