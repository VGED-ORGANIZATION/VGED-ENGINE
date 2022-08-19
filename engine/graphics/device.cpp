#include "device.hpp"
#include "../core/debug.hpp"

#include <vulkan/vulkan_core.h>

#include <stdio.h>
#include <string>
#include <cstring>
#include <cstddef>
#include <unordered_set>
#include <set> 
#include <limits>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
                std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
                return VK_FALSE;
            }

            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
                auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
                } else {
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
                }
            }

            void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
                auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    func(instance, debugMessenger, pAllocator);
                }
            }

            Device::Device(Window *_window) : window{_window} {
                create_instance();
                create_surface();
                pick_physical_device();
                create_logical_device();
                create_vma_allocator();
                create_command_pool();
            }

            Device::~Device() {
                vmaDestroyAllocator(vma_allocator);
                vkDestroyCommandPool(vk_device, vk_command_pool, nullptr);
                vkDestroyDevice(vk_device, nullptr);

                if (enable_validation_layers) {
                    DestroyDebugUtilsMessengerEXT(vk_instance, vk_debug_messenger, nullptr);
                }

                vkDestroySurfaceKHR(vk_instance, vk_surface_khr, nullptr);
                vkDestroyInstance(vk_instance, nullptr);
            }

            void Device::create_instance() {
                volkInitialize();

                if (enable_validation_layers && !check_validation_layer_support()) {
                    throw std::runtime_error("validation layers requested, but not available!");
                }

                const VkApplicationInfo vk_application_info = {
                    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                    .pNext = nullptr,
                    .pApplicationName = "VGED Engine App",
                    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                    .pEngineName = "VGED Engine",
                    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                    .apiVersion = VK_API_VERSION_1_3,
                };

                VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info = {
                    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                    .pNext = nullptr,
                    .flags = 0,
                    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                    .pfnUserCallback = debugCallback,
                    .pUserData = nullptr
                };

                auto extensions = get_required_extensions();

                VkInstanceCreateInfo vk_instance_create_info = {
                    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                    .pNext = nullptr,
                    .pApplicationInfo = &vk_application_info,
                    .enabledLayerCount = static_cast<u32>(validation_layers.size()),
                    .ppEnabledLayerNames = validation_layers.data(),
                    .enabledExtensionCount = static_cast<u32>(extensions.size()),
                    .ppEnabledExtensionNames = extensions.data(),
                };

                if (vkCreateInstance(&vk_instance_create_info, nullptr, &vk_instance) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create instance!");
                }

                volkLoadInstance(vk_instance);

                if(enable_validation_layers) {
                    if (CreateDebugUtilsMessengerEXT(vk_instance, &vk_debug_utils_messenger_create_info, nullptr, &vk_debug_messenger) != VK_SUCCESS) {
                        throw std::runtime_error("failed to set up debug messenger!");
                    }
                }
            }

            void Device::pick_physical_device() {
                u32 device_count = 0;
                vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);
                if (device_count == 0) {
                    throw std::runtime_error("failed to find GPUs with Vulkan support!");
                }
                std::cout << "Device count: " << device_count << std::endl;
                std::vector<VkPhysicalDevice> devices(device_count);
                vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());

                for (const auto &device : devices) {
                    if (is_device_suitable(device)) {
                        vk_physical_device = device;
                        break;
                    }
                }

                if (vk_physical_device == VK_NULL_HANDLE) {
                    throw std::runtime_error("failed to find a suitable GPU!");
                }

                vkGetPhysicalDeviceProperties(vk_physical_device, &properties);
                std::cout << "physical device: " << properties.deviceName << std::endl;
            }

            void Device::create_logical_device() {
                QueueFamilyIndices indices = find_queue_families(vk_physical_device);

                std::vector<VkDeviceQueueCreateInfo> vk_device_queue_create_infos;
                std::set<u32> unique_queue_families = {indices.graphics_family, indices.present_family};

                float queue_priority = 1.0f;
                for (u32 queue_family : unique_queue_families) {
                    VkDeviceQueueCreateInfo vk_device_queue_create_info = {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .queueFamilyIndex = queue_family,
                        .queueCount = 1,
                        .pQueuePriorities = &queue_priority
                    };

                    vk_device_queue_create_infos.push_back(vk_device_queue_create_info);
                }

                VkPhysicalDeviceFeatures vk_physical_device_features = {};
                vk_physical_device_features.samplerAnisotropy = VK_TRUE;
                vk_physical_device_features.geometryShader = VK_TRUE;
                vk_physical_device_features.imageCubeArray = VK_TRUE;

                VkDeviceCreateInfo vk_device_create_info = {};
                vk_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                vk_device_create_info.queueCreateInfoCount = static_cast<u32>(vk_device_queue_create_infos.size());
                vk_device_create_info.pQueueCreateInfos = vk_device_queue_create_infos.data();
                vk_device_create_info.pEnabledFeatures = &vk_physical_device_features;
                vk_device_create_info.enabledExtensionCount = static_cast<u32>(device_extensions.size());
                vk_device_create_info.ppEnabledExtensionNames = device_extensions.data();

                if (enable_validation_layers) {
                    vk_device_create_info.enabledLayerCount = static_cast<u32>(validation_layers.size());
                    vk_device_create_info.ppEnabledLayerNames = validation_layers.data();
                } else {
                    vk_device_create_info.enabledLayerCount = 0;
                }

                if (vkCreateDevice(vk_physical_device, &vk_device_create_info, nullptr, &vk_device) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create logical device!");
                }

                volkLoadDevice(vk_device);
                volkLoadDeviceTable(&volk_device_table, vk_device);

                vkGetDeviceQueue(vk_device, indices.graphics_family, 0, &vk_graphics_queue);
                vkGetDeviceQueue(vk_device, indices.present_family, 0, &vk_present_queue);
            }

            void Device::create_vma_allocator() {
                VmaVulkanFunctions vma_vulkan_functions = {
                    .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
                    .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
                    .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
                    .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
                    .vkAllocateMemory = volk_device_table.vkAllocateMemory,
                    .vkFreeMemory = volk_device_table.vkFreeMemory,
                    .vkMapMemory = volk_device_table.vkMapMemory,
                    .vkUnmapMemory = volk_device_table.vkUnmapMemory,
                    .vkFlushMappedMemoryRanges = volk_device_table.vkFlushMappedMemoryRanges,
                    .vkInvalidateMappedMemoryRanges = volk_device_table.vkInvalidateMappedMemoryRanges,
                    .vkBindBufferMemory = volk_device_table.vkBindBufferMemory,
                    .vkBindImageMemory = volk_device_table.vkBindImageMemory,
                    .vkGetBufferMemoryRequirements = volk_device_table.vkGetBufferMemoryRequirements,
                    .vkGetImageMemoryRequirements = volk_device_table.vkGetImageMemoryRequirements,
                    .vkCreateBuffer = volk_device_table.vkCreateBuffer,
                    .vkDestroyBuffer = volk_device_table.vkDestroyBuffer,
                    .vkCreateImage = volk_device_table.vkCreateImage,
                    .vkDestroyImage = volk_device_table.vkDestroyImage,
                    .vkCmdCopyBuffer = volk_device_table.vkCmdCopyBuffer,
                    .vkGetBufferMemoryRequirements2KHR = volk_device_table.vkGetBufferMemoryRequirements2KHR,
                    .vkGetImageMemoryRequirements2KHR = volk_device_table.vkGetImageMemoryRequirements2KHR,
                    .vkBindBufferMemory2KHR = volk_device_table.vkBindBufferMemory2KHR,
                    .vkBindImageMemory2KHR = volk_device_table.vkBindImageMemory2KHR,
                    .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR,
                    .vkGetDeviceBufferMemoryRequirements = volk_device_table.vkGetDeviceBufferMemoryRequirements,
                    .vkGetDeviceImageMemoryRequirements = volk_device_table.vkGetDeviceImageMemoryRequirements,
                };

                VmaAllocatorCreateInfo vma_allocator_create_info{
                    .flags = {},
                    .physicalDevice = vk_physical_device,
                    .device = vk_device,
                    .preferredLargeHeapBlockSize = 0, // Sets it to lib internal default (256MiB).
                    .pAllocationCallbacks = nullptr,
                    .pDeviceMemoryCallbacks = nullptr,
                    .pHeapSizeLimit = nullptr,
                    .pVulkanFunctions = &vma_vulkan_functions,
                    .instance = vk_instance,
                    .vulkanApiVersion = VK_API_VERSION_1_3,
                };

                vmaCreateAllocator(&vma_allocator_create_info, &vma_allocator);
            }

            void Device::create_command_pool() {
                QueueFamilyIndices queue_family_indices = find_physical_queue_families();

                VkCommandPoolCreateInfo vk_command_pool_create_info = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                    .queueFamilyIndex = queue_family_indices.graphics_family
                };

                if (vkCreateCommandPool(vk_device, &vk_command_pool_create_info, nullptr, &vk_command_pool) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create command pool!");
                }
            }

            void Device::create_surface() { window->create_window_surface(vk_instance, &vk_surface_khr); }

            bool Device::is_device_suitable(VkPhysicalDevice device) {
                QueueFamilyIndices indices = find_queue_families(device);

                bool extensions_supported = check_device_extension_support(device);

                bool swapchain_adequate = false;
                if (extensions_supported) {
                    SwapChainSupportDetails swapchain_support = query_swapchain_support(device);
                    swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
                }

                VkPhysicalDeviceFeatures supported_features;
                vkGetPhysicalDeviceFeatures(device, &supported_features);

                return indices.is_complete() && extensions_supported && swapchain_adequate && supported_features.samplerAnisotropy;
            }

            bool Device::check_validation_layer_support() {
                u32 layer_count;
                vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

                std::vector<VkLayerProperties> available_layers(layer_count);
                vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

                for (const char *layer_name : validation_layers) {
                    bool layer_found = false;

                    for (const auto &layer_properties: available_layers) {
                        if (strcmp(layer_name, layer_properties.layerName) == 0) {
                            layer_found = true;
                            break;
                        }
                    }

                    if (!layer_found) {
                        return false;
                    }
                }

                return true;
            }

            std::vector<const char *> Device::get_required_extensions() const {
                u32 glfw_extension_count = 0;
                const char **glfw_extensions;
                glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

                std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

                if (enable_validation_layers) {
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                }

                return extensions;
            }

            bool Device::check_device_extension_support(VkPhysicalDevice device) {
                u32 extension_count;
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

                std::vector<VkExtensionProperties> available_extensions(extension_count);
                vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

                std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

                for (const auto &extension : available_extensions) {
                    required_extensions.erase(extension.extensionName);
                }

                return required_extensions.empty();
            }

            QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device) const {
                QueueFamilyIndices indices;

                u32 queue_family_count = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

                std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

                int i = 0;
                for (const auto &queue_family : queue_families) {
                    if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        indices.graphics_family = i;
                        indices.graphics_family_has_value = true;
                    }
                    VkBool32 present_support = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface_khr, &present_support);
                    if (queue_family.queueCount > 0 && present_support) {
                        indices.present_family = i;
                        indices.present_family_has_value = true;
                    }
                    if (indices.is_complete()) {
                        break;
                    }

                    i++;
                }

                return indices;
            }

            SwapChainSupportDetails Device::query_swapchain_support(VkPhysicalDevice device) const {
                SwapChainSupportDetails details;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface_khr, &details.capabilities);

                u32 format_count;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface_khr, &format_count, nullptr);

                if (format_count != 0) {
                    details.formats.resize(format_count);
                    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface_khr, &format_count, details.formats.data());
                }

                u32 present_mode_count;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface_khr, &present_mode_count, nullptr);

                if (present_mode_count != 0) {
                    details.present_modes.resize(present_mode_count);
                    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface_khr, &present_mode_count, details.present_modes.data());
                }
                return details;
            }

            VkFormat Device::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
                for (VkFormat format : candidates) {
                    VkFormatProperties props;
                    vkGetPhysicalDeviceFormatProperties(vk_physical_device, format, &props);

                    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                        return format;
                    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                        return format;
                    }
                }
                throw std::runtime_error("failed to find supported format!");
            }

            u32 Device::find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties) const {
                VkPhysicalDeviceMemoryProperties memory_properties;
                vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &memory_properties);
                for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
                    if ((type_filter & (1 << i)) &&
                        (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                        return i;
                    }
                }

                throw std::runtime_error("failed to find suitable memory type!");
            }

            void Device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, MemoryFlags memory_flags, VkBuffer &vk_buffer, VmaAllocation &vma_allocation) {
                VkBufferCreateInfo vk_buffer_create_info = {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .size = size,
                    .usage = usage,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .queueFamilyIndexCount = 0,
                    .pQueueFamilyIndices = nullptr
                };

                VmaAllocationCreateInfo vma_allocation_create_info = {
                    .flags = static_cast<VmaAllocationCreateFlags>(memory_flags),
                    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                    .memoryTypeBits = std::numeric_limits<u32>::max(),
                    .pool = nullptr,
                    .pUserData = nullptr,
                    .priority = 0.5f,
                };

                vmaCreateBuffer(vma_allocator, &vk_buffer_create_info, &vma_allocation_create_info, &vk_buffer, &vma_allocation, nullptr);
            }

            VkCommandBuffer Device::begin_single_time_command_buffer() {
                VkCommandBufferAllocateInfo vk_command_buffer_allocate_info = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                    .pNext = nullptr,
                    .commandPool = vk_command_pool,
                    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    .commandBufferCount = 1,
                };

                VkCommandBuffer command_buffer;
                vkAllocateCommandBuffers(vk_device, &vk_command_buffer_allocate_info, &command_buffer);

                VkCommandBufferBeginInfo vk_command_buffer_begin_info = {
                    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                    .pNext = nullptr,
                    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                    .pInheritanceInfo = nullptr
                };

                vkBeginCommandBuffer(command_buffer, &vk_command_buffer_begin_info);
                return command_buffer;
            }

            void Device::end_single_time_command_buffer(VkCommandBuffer command_buffer) const {
                vkEndCommandBuffer(command_buffer);

                VkSubmitInfo vk_submit_info = {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = nullptr,
                    .waitSemaphoreCount = 0,
                    .pWaitSemaphores = nullptr,
                    .pWaitDstStageMask = nullptr,
                    .commandBufferCount = 1,
                    .pCommandBuffers = &command_buffer,
                    .signalSemaphoreCount = 0,
                    .pSignalSemaphores = nullptr
                };

                vkQueueSubmit(vk_graphics_queue, 1, &vk_submit_info, VK_NULL_HANDLE);
                vkQueueWaitIdle(vk_graphics_queue);

                vkFreeCommandBuffers(vk_device, vk_command_pool, 1, &command_buffer);
            }

            void Device::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
                VkCommandBuffer command_buffer = begin_single_time_command_buffer();

                VkBufferCopy vk_copy_region = {
                    .srcOffset = 0,
                    .dstOffset = 0,
                    .size = size
                };

                vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &vk_copy_region);

                end_single_time_command_buffer(command_buffer);
            }

            void Device::copy_buffer_to_image(VkBuffer buffer, VkImage image, u32 width, u32 height, u32 layer_count) {
                VkCommandBuffer command_buffer = begin_single_time_command_buffer();

                VkBufferImageCopy vk_buffer_image_copy = {
                    .bufferOffset = 0,
                    .bufferRowLength = 0,
                    .bufferImageHeight = 0,
                    .imageSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = 0,
                        .layerCount = layer_count
                    },
                    .imageOffset = {0, 0, 0},
                    .imageExtent = {width, height, 1}
                };

                vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_buffer_image_copy);
                end_single_time_command_buffer(command_buffer);
            }

            void Device::create_image_with_info(const VkImageCreateInfo &vk_image_create_info, MemoryFlags memory_flags, VkImage &vk_image, VmaAllocation &vma_allocation) {
                VmaAllocationCreateInfo vma_allocation_create_info{
                    .flags = static_cast<VmaAllocationCreateFlags>(memory_flags),
                    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                    .memoryTypeBits = std::numeric_limits<u32>::max(),
                    .pool = nullptr,
                    .pUserData = nullptr,
                    .priority = 0.5f,
                };

                vmaCreateImage(this->vma_allocator, &vk_image_create_info, &vma_allocation_create_info, &vk_image, &vma_allocation, nullptr);
            }
        }
    }
}