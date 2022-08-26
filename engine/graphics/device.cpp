#include "device.hpp"
#include "../core/window.hpp"

#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <limits>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
            static constexpr uint32_t VULKAN_API = VK_API_VERSION_1_3;

			// local callback functions
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
																void *pUserData) {
				std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

				return VK_FALSE;
			}

			VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
				if (func != nullptr) {
					return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
				} else {
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}

			void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
				if (func != nullptr) {
					func(instance, debugMessenger, pAllocator);
				}
			}

			// class member functions
			Device::Device(VGED::Engine::Window &_window) : window{ _window } {
				create_instance();
				setup_debug_messenger();
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
					DestroyDebugUtilsMessengerEXT(vk_instance, vk_debug_utils_messenger_ext, nullptr);
				}

				vkDestroySurfaceKHR(vk_instance, vk_surface_khr, nullptr);
				vkDestroyInstance(vk_instance, nullptr);
			}

			void Device::create_instance() {
				volkInitialize();

				if (enable_validation_layers && !check_validation_layer_support()) {
					throw std::runtime_error("validation layers requested, but not available!");
				}

				VkApplicationInfo appInfo = {};
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pApplicationName = "LittleVulkanEngine App";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = "No Engine";
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VULKAN_API;

				VkInstanceCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				createInfo.pApplicationInfo = &appInfo;

				auto extensions = get_required_extensions();
				createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
				createInfo.ppEnabledExtensionNames = extensions.data();

				VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
				if (enable_validation_layers) {
					createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
					createInfo.ppEnabledLayerNames = validation_layers.data();

					populate_debug_messenger_create_info(debugCreateInfo);
					createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
				} else {
					createInfo.enabledLayerCount = 0;
					createInfo.pNext = nullptr;
				}

				if (vkCreateInstance(&createInfo, nullptr, &vk_instance) != VK_SUCCESS) {
					throw std::runtime_error("failed to create instance!");
				}

				volkLoadInstance(vk_instance);

				has_gflw_required_instance_extensions();
			}

			void Device::pick_physical_device() {
				uint32_t deviceCount = 0;
				vkEnumeratePhysicalDevices(vk_instance, &deviceCount, nullptr);
				if (deviceCount == 0) {
					throw std::runtime_error("failed to find GPUs with Vulkan support!");
				}
				std::cout << "Device count: " << deviceCount << std::endl;
				std::vector<VkPhysicalDevice> devices(deviceCount);
				vkEnumeratePhysicalDevices(vk_instance, &deviceCount, devices.data());

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

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				std::set<uint32_t> uniqueQueueFamilies = { indices.graphics_family, indices.present_family };

				float queuePriority = 1.0f;
				for (uint32_t queueFamily : uniqueQueueFamilies) {
					VkDeviceQueueCreateInfo queueCreateInfo = {};
					queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.queueFamilyIndex = queueFamily;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}

				VkPhysicalDeviceFeatures deviceFeatures = {};
				deviceFeatures.samplerAnisotropy = VK_TRUE;

				VkDeviceCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

				createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
				createInfo.pQueueCreateInfos = queueCreateInfos.data();

				createInfo.pEnabledFeatures = &deviceFeatures;
				createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
				createInfo.ppEnabledExtensionNames = device_extensions.data();

				// might not really be necessary anymore because device specific validation layers
				// have been deprecated
				if (enable_validation_layers) {
					createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
					createInfo.ppEnabledLayerNames = validation_layers.data();
				} else {
					createInfo.enabledLayerCount = 0;
				}

				if (vkCreateDevice(vk_physical_device, &createInfo, nullptr, &vk_device) != VK_SUCCESS) {
					throw std::runtime_error("failed to create logical device!");
				}

				volkLoadDevice(vk_device);
				//volkLoadDeviceTable(&volk_device_table, vk_device);

				vkGetDeviceQueue(vk_device, indices.graphics_family, 0, &vk_graphics_queue);
				vkGetDeviceQueue(vk_device, indices.present_family, 0, &vk_present_queue);
			}

			void Device::create_vma_allocator() {
                /*VmaVulkanFunctions vma_vulkan_functions{};
                vma_vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
                vma_vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;*/

                VmaVulkanFunctions vma_vulkan_functions = {
                    .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
                    .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
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
					.vulkanApiVersion = VULKAN_API,
				};

				vmaCreateAllocator(&vma_allocator_create_info, &this->vma_allocator);
			}

			void Device::create_command_pool() {
				QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

				VkCommandPoolCreateInfo poolInfo = {};
				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family;
				poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

				if (vkCreateCommandPool(vk_device, &poolInfo, nullptr, &vk_command_pool) != VK_SUCCESS) {
					throw std::runtime_error("failed to create command pool!");
				}
			}

			void Device::create_surface() { window.create_window_surface(vk_instance, &vk_surface_khr); }

			bool Device::is_device_suitable(VkPhysicalDevice device) {
				QueueFamilyIndices indices = find_queue_families(device);

				bool extensionsSupported = check_device_extension_support(device);

				bool swapChainAdequate = false;
				if (extensionsSupported) {
					SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
					swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
				}

				VkPhysicalDeviceFeatures supportedFeatures;
				vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

				return indices.is_complete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
			}

			void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
				createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = debugCallback;
				createInfo.pUserData = nullptr; // Optional
			}

			void Device::setup_debug_messenger() {
				if (!enable_validation_layers)
					return;
				VkDebugUtilsMessengerCreateInfoEXT createInfo;
				populate_debug_messenger_create_info(createInfo);
				if (CreateDebugUtilsMessengerEXT(vk_instance, &createInfo, nullptr, &vk_debug_utils_messenger_ext) != VK_SUCCESS) {
					throw std::runtime_error("failed to set up debug messenger!");
				}
			}

			bool Device::check_validation_layer_support() {
				uint32_t layerCount;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				for (const char *layerName : validation_layers) {
					bool layerFound = false;

					for (const auto &layerProperties : availableLayers) {
						if (strcmp(layerName, layerProperties.layerName) == 0) {
							layerFound = true;
							break;
						}
					}

					if (!layerFound) {
						return false;
					}
				}

				return true;
			}

			std::vector<const char *> Device::get_required_extensions() {
				uint32_t glfwExtensionCount = 0;
				const char **glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

				if (enable_validation_layers) {
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}

				return extensions;
			}

			void Device::has_gflw_required_instance_extensions() {
				uint32_t extensionCount = 0;
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> extensions(extensionCount);
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

				std::cout << "available extensions:" << std::endl;
				std::unordered_set<std::string> available;
				for (const auto &extension : extensions) {
					std::cout << "\t" << extension.extensionName << std::endl;
					available.insert(extension.extensionName);
				}

				std::cout << "required extensions:" << std::endl;
				auto requiredExtensions = get_required_extensions();
				for (const auto &required : requiredExtensions) {
					std::cout << "\t" << required << std::endl;
					if (available.find(required) == available.end()) {
						throw std::runtime_error("Missing required glfw extension");
					}
				}
			}

			bool Device::check_device_extension_support(VkPhysicalDevice device) {
				uint32_t extensionCount;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

				for (const auto &extension : availableExtensions) {
					requiredExtensions.erase(extension.extensionName);
				}

				return requiredExtensions.empty();
			}

			QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice device) {
				QueueFamilyIndices indices;

				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

				int i = 0;
				for (const auto &queueFamily : queueFamilies) {
					if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						indices.graphics_family = i;
						indices.graphics_family_has_value = true;
					}
					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface_khr, &presentSupport);
					if (queueFamily.queueCount > 0 && presentSupport) {
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

			SwapChainSupportDetails Device::query_swap_chain_support(VkPhysicalDevice device) {
				SwapChainSupportDetails details;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk_surface_khr, &details.capabilities);

				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface_khr, &formatCount, nullptr);

				if (formatCount != 0) {
					details.formats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk_surface_khr, &formatCount, details.formats.data());
				}

				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface_khr, &presentModeCount, nullptr);

				if (presentModeCount != 0) {
					details.present_modes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk_surface_khr, &presentModeCount, details.present_modes.data());
				}
				return details;
			}

			VkFormat Device::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
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

			uint32_t Device::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &memProperties);
				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
					if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
						return i;
					}
				}

				throw std::runtime_error("failed to find suitable memory type!");
			}

			VkCommandBuffer Device::begin_single_time_commands() {
				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = vk_command_pool;
				allocInfo.commandBufferCount = 1;

				VkCommandBuffer commandBuffer;
				vkAllocateCommandBuffers(vk_device, &allocInfo, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(commandBuffer, &beginInfo);
				return commandBuffer;
			}

			void Device::end_single_time_commands(VkCommandBuffer commandBuffer) {
				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(vk_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(vk_graphics_queue);

				vkFreeCommandBuffers(vk_device, vk_command_pool, 1, &commandBuffer);
			}

			void Device::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
				VkCommandBuffer commandBuffer = begin_single_time_commands();

				VkBufferCopy copyRegion{};
				copyRegion.srcOffset = 0; // Optional
				copyRegion.dstOffset = 0; // Optional
				copyRegion.size = size;
				vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

				end_single_time_commands(commandBuffer);
			}

			void Device::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
				VkCommandBuffer commandBuffer = begin_single_time_commands();

				VkBufferImageCopy region{};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;

				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = layerCount;

				region.imageOffset = { 0, 0, 0 };
				region.imageExtent = { width, height, 1 };

				vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				end_single_time_commands(commandBuffer);
			}

			void Device::create_image_with_info(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) {
				if (vkCreateImage(vk_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
					throw std::runtime_error("failed to create image!");
				}

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(vk_device, image, &memRequirements);

				VkMemoryAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate image memory!");
				}

				if (vkBindImageMemory(vk_device, image, imageMemory, 0) != VK_SUCCESS) {
					throw std::runtime_error("failed to bind image memory!");
				}
			}
		}
	}
}