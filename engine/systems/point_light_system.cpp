#include "point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

namespace VGED {
	namespace Engine {
		inline namespace System {
			struct PointLightPushConstants {
				glm::vec4 position{};
				glm::vec4 color{};
				float radius;
			};

			PointLightSystem::PointLightSystem(Device &_device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device{ _device } {
				pipeline = std::make_unique<RasterPipeline>(device, RasterPipelineInfo {
					.vertex_shader_info = {.source = ShaderFile {"shaders/point_light.vert"}},
					.fragment_shader_info = { .source = ShaderFile {"shaders/point_light.frag"}},
					.color_attachments = {
						{ 
							.format = ImageFormat::B8G8R8A8_SRGB, 
							.blend = {
								.blend_enable = true,
								.src_color_blend_factor = BlendFactor::SRC_ALPHA,
								.dst_color_blend_factor = BlendFactor::ONE_MINUS_SRC_ALPHA,
							} 
						}
					},
					.depth_test = {
						.enable_depth_test = true,
						.enable_depth_write = true,
					},
					.vk_render_pass = renderPass,
					.pipeline_layout_info = {
						.push_constant_size = sizeof(PointLightPushConstants),
						.vk_descriptor_set_layouts = { globalSetLayout }
					},
					.vertex_input = { .binding = {}, .attribute = {} }
				});
			}

			PointLightSystem::~PointLightSystem() {}

			void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
				auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frameTime, { 0.f, -1.f, 0.f });
				int lightIndex = 0;
				for (auto &kv : frameInfo.gameObjects) {
					auto &obj = kv.second;
					if (obj.pointLight == nullptr)
						continue;

					assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

					// update light position
					obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

					// copy light to ubo
					ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
					ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

					lightIndex += 1;
				}
				ubo.numLights = lightIndex;
			}

			void PointLightSystem::render(FrameInfo &frameInfo) {
				// sort lights
				std::map<float, GameObject::id_t> sorted;
				for (auto &kv : frameInfo.gameObjects) {
					auto &obj = kv.second;
					if (obj.pointLight == nullptr)
						continue;

					// calculate distance
					auto offset = frameInfo.camera.getPosition() - obj.transform.translation;
					float disSquared = glm::dot(offset, offset);
					sorted[disSquared] = obj.getId();
				}

				pipeline->bind(frameInfo.commandBuffer);

				vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline_layout(), 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

				// iterate through sorted lights in reverse order
				for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
					// use game obj id to find light object
					auto &obj = frameInfo.gameObjects.at(it->second);

					PointLightPushConstants push{};
					push.position = glm::vec4(obj.transform.translation, 1.f);
					push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
					push.radius = obj.transform.scale.x;

					vkCmdPushConstants(frameInfo.commandBuffer, pipeline->pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);
					vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
				}
			}
		}
	}
}
