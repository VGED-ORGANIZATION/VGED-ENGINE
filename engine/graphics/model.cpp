#include "model.hpp"

#include "descriptors.hpp"
#include "device.hpp"
#include "utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtc/type_ptr.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <filesystem>
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STBI_MSC_SECURE_CRT
#include <tinygltf/tiny_gltf.h>

#ifndef ENGINE_DIR
#define ENGINE_DIR ""
#endif

namespace std {
	template <> struct hash<VGED::Engine::Graphics::Model::Vertex> {
		size_t operator()(VGED::Engine::Graphics::Model::Vertex const &vertex) const {
			size_t seed = 0;
			VGED::Engine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
				uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
				assert(vertexCount >= 3 && "Vertex count must be at least 3");
				VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
				uint32_t vertexSize = sizeof(vertices[0]);

				Buffer stagingBuffer{ device, vertexSize, vertexCount, MemoryFlagBits::HOST_ACCESS_SEQUENTIAL_WRITE};

				stagingBuffer.map();
				stagingBuffer.write_to_buffer((void *)vertices.data());

				vertexBuffer = std::make_unique<Buffer>(device, vertexSize, vertexCount, MemoryFlagBits::DEDICATED_MEMORY);

				device.copy_buffer(stagingBuffer.get_buffer(), vertexBuffer->get_buffer(), bufferSize);
			}

			void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
				uint32_t indexCount = static_cast<uint32_t>(indices.size());
				hasIndexBuffer = indexCount > 0;

				if (!hasIndexBuffer) {
					return;
				}

				VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
				uint32_t indexSize = sizeof(indices[0]);

				Buffer stagingBuffer{ device, indexSize, indexCount, MemoryFlagBits::HOST_ACCESS_SEQUENTIAL_WRITE };

				stagingBuffer.map();
				stagingBuffer.write_to_buffer((void *)indices.data());

				indexBuffer = std::make_unique<Buffer>(device, indexSize, indexCount, MemoryFlagBits::DEDICATED_MEMORY);

				device.copy_buffer(stagingBuffer.get_buffer(), indexBuffer->get_buffer(), bufferSize);
			}

			void Model::draw(VkCommandBuffer commandBuffer, VkDescriptorSet globalDescriptorSet, VkPipelineLayout pipelineLayout) {
				for (auto &primitive : primitives) {
					if (hasIndexBuffer) {
						std::vector<VkDescriptorSet> sets{ globalDescriptorSet, primitive.material.descriptorSet };
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, sets.size(), sets.data(), 0, nullptr);
						vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, primitive.firstVertex, 0);
					} else {
						vkCmdDraw(commandBuffer, primitive.vertexCount, 1, 0, 0);
					}
				}
			}

			void Model::bind(VkCommandBuffer commandBuffer) {
				VkBuffer buffers[] = { vertexBuffer->get_buffer() };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

				if (hasIndexBuffer) {
					vkCmdBindIndexBuffer(commandBuffer, indexBuffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
				}
			}

			std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
				std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(Vertex);
				bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescriptions;
			}

			std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
				std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
				attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
				attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
				attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
				attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent) });
				attributeDescriptions.push_back({ 4, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

				return attributeDescriptions;
			}

			Model::~Model() {}

			Model::Model(Device &_device, const std::string &filepath, DescriptorSetLayout &materialSetLayout, DescriptorPool &descriptorPool) : device{ _device } {
				std::string warn, err;
				tinygltf::TinyGLTF GltfLoader;
				tinygltf::Model GltfModel;
				if (!GltfLoader.LoadASCIIFromFile(&GltfModel, &err, &warn, filepath)) {
					throw std::runtime_error("failed to load gltf file!");
				}

				auto path = std::filesystem::path{ filepath };

				for (auto &image : GltfModel.images) {
					images.push_back(std::make_shared<Texture>(device, path.parent_path().append(image.uri).generic_string()));
				}

				for (auto &scene : GltfModel.scenes) {
					for (size_t i = 0; i < scene.nodes.size(); i++) {
						auto &node = GltfModel.nodes[i];
						uint32_t vertexOffset = 0;
						uint32_t indexOffset = 0;

						for (auto &GltfPrimitive : GltfModel.meshes[node.mesh].primitives) {
							uint32_t vertexCount = 0;
							uint32_t indexCount = 0;

							const float *positionBuffer = nullptr;
							const float *normalsBuffer = nullptr;
							const float *texCoordsBuffer = nullptr;
							const float *tangentsBuffer = nullptr;

							if (GltfPrimitive.attributes.find("POSITION") != GltfPrimitive.attributes.end()) {
								const tinygltf::Accessor &accessor = GltfModel.accessors[GltfPrimitive.attributes.find("POSITION")->second];
								const tinygltf::BufferView &view = GltfModel.bufferViews[accessor.bufferView];
								positionBuffer = reinterpret_cast<const float *>(&(GltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
								vertexCount = accessor.count;
							}

							if (GltfPrimitive.attributes.find("NORMAL") != GltfPrimitive.attributes.end()) {
								const tinygltf::Accessor &accessor = GltfModel.accessors[GltfPrimitive.attributes.find("NORMAL")->second];
								const tinygltf::BufferView &view = GltfModel.bufferViews[accessor.bufferView];
								normalsBuffer = reinterpret_cast<const float *>(&(GltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
							}

							if (GltfPrimitive.attributes.find("TEXCOORD_0") != GltfPrimitive.attributes.end()) {
								const tinygltf::Accessor &accessor = GltfModel.accessors[GltfPrimitive.attributes.find("TEXCOORD_0")->second];
								const tinygltf::BufferView &view = GltfModel.bufferViews[accessor.bufferView];
								texCoordsBuffer = reinterpret_cast<const float *>(&(GltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
							}

							if (GltfPrimitive.attributes.find("TANGENT") != GltfPrimitive.attributes.end()) {
								const tinygltf::Accessor &accessor = GltfModel.accessors[GltfPrimitive.attributes.find("TANGENT")->second];
								const tinygltf::BufferView &view = GltfModel.bufferViews[accessor.bufferView];
								tangentsBuffer = reinterpret_cast<const float *>(&(GltfModel.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
							}

							for (size_t v = 0; v < vertexCount; v++) {
								Vertex vertex{};
								vertex.position = glm::make_vec3(&positionBuffer[v * 3]);
								vertex.color = glm::vec3(1.0);
								vertex.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
								vertex.tangent = glm::vec4(tangentsBuffer ? glm::make_vec4(&tangentsBuffer[v * 4]) : glm::vec4(0.0f));
								;
								vertex.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
								vertices.push_back(vertex);
							}

							{
								const tinygltf::Accessor &accessor = GltfModel.accessors[GltfPrimitive.indices];
								const tinygltf::BufferView &bufferView = GltfModel.bufferViews[accessor.bufferView];
								const tinygltf::Buffer &buffer = GltfModel.buffers[bufferView.buffer];

								indexCount += static_cast<uint32_t>(accessor.count);

								switch (accessor.componentType) {
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
									const uint32_t *buf = reinterpret_cast<const uint32_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++) {
										indices.push_back(buf[index]);
									}
									break;
								}
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
									const uint16_t *buf = reinterpret_cast<const uint16_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++) {
										indices.push_back(buf[index]);
									}
									break;
								}
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
									const uint8_t *buf = reinterpret_cast<const uint8_t *>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++) {
										indices.push_back(buf[index]);
									}
									break;
								}
								default: std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl; return;
								}
							}

							std::shared_ptr<Texture> defaultTexture = std::make_shared<Texture>(device, "textures/white.png");

							Material material{};
							if (GltfPrimitive.material != -1) {
								tinygltf::Material &GltfPrimitiveMaterial = GltfModel.materials[GltfPrimitive.material];

								if (GltfPrimitiveMaterial.pbrMetallicRoughness.baseColorTexture.index != -1) {
									uint32_t textureIndex = GltfPrimitiveMaterial.pbrMetallicRoughness.baseColorTexture.index;
									uint32_t imageIndex = GltfModel.textures[textureIndex].source;
									material.albedoTexture = images[imageIndex];
								} else {
									material.albedoTexture = defaultTexture;
								}

								if (GltfPrimitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
									uint32_t textureIndex = GltfPrimitiveMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
									uint32_t imageIndex = GltfModel.textures[textureIndex].source;
									material.metallicRoughnessTexture = images[imageIndex];
								} else {
									material.metallicRoughnessTexture = defaultTexture;
								}

								if (GltfPrimitiveMaterial.normalTexture.index != -1) {
									uint32_t textureIndex = GltfPrimitiveMaterial.normalTexture.index;
									uint32_t imageIndex = GltfModel.textures[textureIndex].source;
									material.normalTexture = images[imageIndex];
								} else {
									material.normalTexture = defaultTexture;
								}
							} else {
								material.albedoTexture = defaultTexture;
								material.normalTexture = defaultTexture;
								material.metallicRoughnessTexture = defaultTexture;
							}

							VkDescriptorImageInfo albedo_info = {};
							albedo_info.sampler = material.albedoTexture->getSampler();
							albedo_info.imageView = material.albedoTexture->getImageView();
							albedo_info.imageLayout = material.albedoTexture->getImageLayout();

							VkDescriptorImageInfo normal_info = {};
							normal_info.sampler = material.normalTexture->getSampler();
							normal_info.imageView = material.normalTexture->getImageView();
							normal_info.imageLayout = material.normalTexture->getImageLayout();

							VkDescriptorImageInfo metallicRoughness_info = {};
							metallicRoughness_info.sampler = material.metallicRoughnessTexture->getSampler();
							metallicRoughness_info.imageView = material.metallicRoughnessTexture->getImageView();
							metallicRoughness_info.imageLayout = material.metallicRoughnessTexture->getImageLayout();

							DescriptorWriter(materialSetLayout, descriptorPool).writeImage(0, &albedo_info).writeImage(1, &normal_info).writeImage(2, &metallicRoughness_info).build(material.descriptorSet);

							Primitive primitive{};
							primitive.firstVertex = vertexOffset;
							primitive.vertexCount = vertexCount;
							primitive.indexCount = indexCount;
							primitive.firstIndex = indexOffset;
							primitive.material = material;
							primitives.push_back(primitive);

							vertexOffset += vertexCount;
							indexOffset += indexCount;
						}
					}

					createVertexBuffers(vertices);
					createIndexBuffers(indices);
				}
			}
		}
	}
}
