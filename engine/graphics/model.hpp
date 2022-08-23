#pragma once

#include "buffer.hpp"
#include "device.hpp"
#include "texture.hpp"
#include "descriptors.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <filesystem>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			class Model {
			public:
				struct Material {
					std::shared_ptr<Texture> albedoTexture;
					std::shared_ptr<Texture> normalTexture;
					std::shared_ptr<Texture> metallicRoughnessTexture;
					VkDescriptorSet descriptorSet;
				};

				struct Primitive {
					uint32_t firstIndex;
					uint32_t firstVertex;
					uint32_t indexCount;
					uint32_t vertexCount;
					Material material;
				};

				struct Vertex {
					glm::vec3 position{};
					glm::vec3 color{};
					glm::vec3 normal{};
					glm::vec4 tangent{};
					glm::vec2 uv{};

					static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

					static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

					bool operator==(const Vertex &other) const { return position == other.position && color == other.color && normal == other.normal && uv == other.uv; }
				};

				Model(Device &_device, const std::string &filepath, DescriptorSetLayout &setLayout, DescriptorPool &pool);
				~Model();

				void bind(VkCommandBuffer commandBuffer);
				void draw(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, VkPipelineLayout pipelineLayout);

			private:
				void createVertexBuffers(const std::vector<Vertex> &vertices);

				void createIndexBuffers(const std::vector<uint32_t> &indices);

				std::unique_ptr<Buffer> vertexBuffer;

				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;
				std::vector<Primitive> primitives;
				std::vector<std::shared_ptr<Texture>> images;

				bool hasIndexBuffer = false;
				std::unique_ptr<Buffer> indexBuffer;
				Device &device;
			};
		}
	}
}