#pragma once

#include "../graphics/camera.hpp"
#include "../graphics/device.hpp"
#include "../graphics/frame_info.hpp"
#include "../graphics/game_object.hpp"
#include "../graphics/pipeline.hpp"

#include <memory>
#include <vector>

namespace VGED {
	namespace Engine {
		inline namespace System {
			class PointLightSystem {
			public:
				PointLightSystem(Device &_device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
				~PointLightSystem();

				PointLightSystem(const PointLightSystem &) = delete;
				PointLightSystem &operator=(const PointLightSystem &) = delete;

				void update(FrameInfo &frameInfo, GlobalUbo &ubo);
				void render(FrameInfo &frameInfo);

			private:
				void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
				void createPipeline(VkRenderPass renderPass);

				Device &device;

				std::unique_ptr<Pipeline> lvePipeline;
				VkPipelineLayout pipelineLayout;
			};
		}
	}
}
