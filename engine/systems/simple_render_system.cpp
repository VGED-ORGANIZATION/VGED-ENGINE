#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace VGED {
	namespace Engine {
		inline namespace System {
			struct SimplePushConstantData {
				glm::mat4 modelMatrix{ 1.f };
				glm::mat4 normalMatrix{ 1.f };
			};

			SimpleRenderSystem::SimpleRenderSystem(Device &_device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> setLayouts) : device{ _device } {
				createPipelineLayout(setLayouts);
				createPipeline(renderPass);
			}

			SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }

			void SimpleRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> setLayouts) {
				VkPushConstantRange pushConstantRange{};
				pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
				pushConstantRange.offset = 0;
				pushConstantRange.size = sizeof(SimplePushConstantData);

				VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
				pipelineLayoutInfo.pSetLayouts = setLayouts.data();
				pipelineLayoutInfo.pushConstantRangeCount = 1;
				pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
				if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
					throw std::runtime_error("failed to create pipeline layout!");
				}
			}

			void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
				assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

				PipelineConfigInfo pipelineConfig{};
				Pipeline::defaultPipelineConfigInfo(pipelineConfig);
				pipelineConfig.renderPass = renderPass;
				pipelineConfig.pipelineLayout = pipelineLayout;
				lvePipeline = std::make_unique<Pipeline>(device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
			}

			void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
				lvePipeline->bind(frameInfo.commandBuffer);

				/*vkCmdBindDescriptorSets(
					frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					pipelineLayout,
					0,
					1,
					&frameInfo.globalDescriptorSet,
					0,
					nullptr);*/

				for (auto &kv : frameInfo.gameObjects) {
					auto &obj = kv.second;
					if (obj.model == nullptr)
						continue;
					SimplePushConstantData push{};
					push.modelMatrix = obj.transform.mat4();
					push.normalMatrix = obj.transform.normalMatrix();

					vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
					obj.model->bind(frameInfo.commandBuffer);
					obj.model->draw(frameInfo.commandBuffer, frameInfo.globalDescriptorSet, pipelineLayout);
				}
			}
		}
	}
}
