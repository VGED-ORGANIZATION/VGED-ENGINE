#include "simple_render_system.hpp"
#include "graphics/pipeline.hpp"
#include <memory>

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
                pipeline = std::make_unique<RasterPipeline>(device, RasterPipelineInfo{
                                                                        .vertex_shader_info = { .source = ShaderFile{ "shaders/simple_shader.vert" } },
                                                                        .fragment_shader_info = { .source = ShaderFile{ "shaders/simple_shader.frag" } },
                                                                        .color_attachments = { { .format = ImageFormat::B8G8R8A8_SRGB, .blend = {} } },
                                                                        .depth_test = {
                                                                            .enable_depth_test = true,
                                                                            .enable_depth_write = true,
                                                                        },
                                                                        .vk_render_pass = renderPass,
                                                                        .pipeline_layout_info = { .push_constant_size = sizeof(SimplePushConstantData), .vk_descriptor_set_layouts = setLayouts } });
            }

            SimpleRenderSystem::~SimpleRenderSystem() {}

            void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
                pipeline->bind(frameInfo.commandBuffer);

                for (auto &kv : frameInfo.gameObjects) {
                    auto &obj = kv.second;
                    if (obj.model == nullptr)
                        continue;
                    SimplePushConstantData push{};
                    push.modelMatrix = obj.transform.mat4();
                    push.normalMatrix = obj.transform.normalMatrix();

                    vkCmdPushConstants(frameInfo.commandBuffer, pipeline->pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
                    obj.model->bind(frameInfo.commandBuffer);
                    obj.model->draw(frameInfo.commandBuffer, frameInfo.globalDescriptorSet, pipeline->pipeline_layout());
                }
            }
        }
    }
}
