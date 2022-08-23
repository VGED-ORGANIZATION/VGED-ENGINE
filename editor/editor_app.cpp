#include "editor_app.hpp"
#include <memory>

#include "../engine/graphics/keyboard_movement_controller.hpp"
#include "../engine/graphics/buffer.hpp"
#include "../engine/graphics/camera.hpp"
#include "../engine/graphics/imgui_layer.hpp"
#include "../engine/systems/point_light_system.hpp"
#include "../engine/systems/simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <filesystem>
#include <iostream>

using namespace VGED::Engine::System;

namespace VGED {
	namespace Editor {
		EditorApp::EditorApp() {
			globalPool = DescriptorPool::Builder(lveDevice).setMaxSets(1000).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000).build();
			loadGameObjects();
		}

		EditorApp::~EditorApp() {}

		void EditorApp::run() {
			std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
			for (auto &uboBuffer : uboBuffers) {
				uboBuffer = std::make_unique<Buffer>(lveDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				uboBuffer->map();
			}

			ImguiLayer lveImgui{lveWindow, lveDevice, lveRenderer.getSwapChainRenderPass(), static_cast<uint32_t>(lveRenderer.getImageCount())};

			texture = std::make_unique<Texture>(lveDevice, "textures/meme.png");

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.sampler = texture->getSampler();
			imageInfo.imageView = texture->getImageView();
			imageInfo.imageLayout = texture->getImageLayout();

			auto globalSetLayout =
				DescriptorSetLayout::Builder(lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT).build();

			auto materialSetLayout = DescriptorSetLayout::Builder(lveDevice)
										 .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
										 .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
										 .addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
										 .build();

			std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
			for (int i = 0; i < globalDescriptorSets.size(); i++) {
				auto bufferInfo = uboBuffers[i]->descriptorInfo();
				DescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).writeImage(1, &imageInfo).build(globalDescriptorSets[i]);
			}

			SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), { globalSetLayout->getDescriptorSetLayout(), materialSetLayout->getDescriptorSetLayout() } };
			PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

			std::shared_ptr<Model> lveModel = std::make_shared<Model>(lveDevice, "models/Sponza/Sponza.gltf", *materialSetLayout, *globalPool);
			auto floor = GameObject::createGameObject();
			floor.model = lveModel;
			floor.transform.translation = { 0.f, .5f, 0.f };
			floor.transform.scale = { .01f, .01f, .01f };
			floor.transform.rotation = { 0.0f, 0.0f, 3.14159265f };
			gameObjects.emplace(floor.getId(), std::move(floor));
			Camera camera{};

			auto viewerObject = GameObject::createGameObject();
			viewerObject.transform.translation.z = -2.5f;
			KeyboardMovementController cameraController{};

			auto currentTime = std::chrono::high_resolution_clock::now();
			while (!lveWindow.should_close()) {
				glfwPollEvents();

				auto newTime = std::chrono::high_resolution_clock::now();
				float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
				currentTime = newTime;

				cameraController.moveInPlaneXZ(lveWindow.get_glfw_window(), frameTime, viewerObject);
				camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

				float aspect = lveRenderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

				if (auto commandBuffer = lveRenderer.beginFrame()) {
					int frameIndex = lveRenderer.getFrameIndex();
					FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects };

					// update
					GlobalUbo ubo{};
					ubo.projection = camera.getProjection();
					ubo.view = camera.getView();
					ubo.inverseView = camera.getInverseView();
					pointLightSystem.update(frameInfo, ubo);
					uboBuffers[frameIndex]->writeToBuffer(&ubo);
					uboBuffers[frameIndex]->flush();

					// render
					lveImgui.newFrame();
					lveRenderer.beginSwapChainRenderPass(commandBuffer);

					// order here matters
					simpleRenderSystem.renderGameObjects(frameInfo);
					pointLightSystem.render(frameInfo);
					lveImgui.runExample();
					lveImgui.render(commandBuffer);

					lveRenderer.endSwapChainRenderPass(commandBuffer);
					lveRenderer.endFrame();
				}
			}

			vkDeviceWaitIdle(lveDevice.device());
		}

		void EditorApp::loadGameObjects() {
			/*std::shared_ptr<Model> lveModel =
				LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
			auto flatVase = LveGameObject::createGameObject();
			flatVase.model = lveModel;
			flatVase.transform.translation = {-.5f, .5f, 0.f};
			flatVase.transform.scale = {3.f, 1.5f, 3.f};
			gameObjects.emplace(flatVase.getId(), std::move(flatVase));

			lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
			auto smoothVase = LveGameObject::createGameObject();
			smoothVase.model = lveModel;
			smoothVase.transform.translation = {.5f, .5f, 0.f};
			smoothVase.transform.scale = {3.f, 1.5f, 3.f};
			gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

			lveModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
			auto floor = LveGameObject::createGameObject();
			floor.model = lveModel;
			floor.transform.translation = {0.f, .5f, 0.f};
			floor.transform.scale = {3.f, .001f, 3.f};
			gameObjects.emplace(floor.getId(), std::move(floor));*/

			std::vector<glm::vec3> lightColors{
				{ 1.f, .1f, .1f }, { .1f, .1f, 1.f }, { .1f, 1.f, .1f }, { 1.f, 1.f, .1f }, { .1f, 1.f, 1.f }, { 1.f, 1.f, 1.f } //
			};

			for (int i = 0; i < lightColors.size(); i++) {
				auto pointLight = GameObject::makePointLight(0.2f);
				pointLight.color = lightColors[i];
				auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), { 0.f, -1.f, 0.f });
				pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
				gameObjects.emplace(pointLight.getId(), std::move(pointLight));
			}
		}
	}
}
