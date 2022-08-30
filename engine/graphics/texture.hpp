#pragma once

#include "device.hpp"
#include "image.hpp"
#include <string.h>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            class Texture {
            public:
                Texture(Device &_device, const std::string &filepath);
                ~Texture();

                Texture(const Texture &) = delete;
                Texture &operator=(const Texture &) = delete;
                Texture(Texture &&) = delete;
                Texture &operator=(Texture &&) = delete;

                VkSampler getSampler() { return device.get_sampler(sampler_id)->sampler(); }
                VkImageView getImageView() { return device.get_image(image_id)->image_view(); }
                VkImageLayout getImageLayout() { return imageLayout; }

            private:
                void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
                void generateMipmaps();

                int width, height, mipLevels;

                Device &device;
                u32 image_id;
                u32 sampler_id;
                VkFormat imageFormat;
                VkImageLayout imageLayout;
            };
        }
    }
}