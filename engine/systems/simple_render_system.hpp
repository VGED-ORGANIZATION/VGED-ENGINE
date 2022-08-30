#pragma once

#include "../graphics/camera.hpp"
#include "../graphics/device.hpp"
#include "../graphics/frame_info.hpp"
#include "../graphics/pipeline.hpp"

#include <memory>
#include <vector>

namespace VGED {
    namespace Engine {
        inline namespace System {
            class SimpleRenderSystem {
            public:
                SimpleRenderSystem(Device &_device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> setLayouts);
                ~SimpleRenderSystem();

                SimpleRenderSystem(const SimpleRenderSystem &) = delete;
                SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

                void renderGameObjects(FrameInfo &frameInfo);

            private:
                Device &device;

                std::unique_ptr<RasterPipeline> pipeline;
            };
        }
    }
}
