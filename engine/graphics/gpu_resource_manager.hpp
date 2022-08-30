#pragma once

#include <array>
#include <memory>
#include <vector>

#include "../core/result.hpp"
#include "../core/types.hpp"
#include "../core/debug.hpp"

#include "image.hpp"
#include "buffer.hpp"

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            template <typename Resource, usize size = 1000>
            class GPUResourcePool {
            public:
                std::pair<std::unique_ptr<Resource> &, u32> create_slot() {
                    if (next_index + 1 == max_resources && free_indices.empty()) {
                        THROW("reached limit of resources!");
                    }

                    u32 index = {};
                    if (free_indices.empty()) {
                        index = next_index;
                        next_index++;
                    } else {
                        index = free_indices.back();
                        free_indices.pop_back();
                    }

                    auto &resource = pool[index];

                    return { resource, index };
                }

                void delete_slot(u32 index) {
                    free_indices.push_back(index);
                }

                bool verify_slot(u32 index) {
                    return pool[index] != nullptr;
                };

                std::unique_ptr<Resource> &get_slot(u32 index) {
                    auto &resource = pool[index];
                    return resource;
                }

            private:
                std::array<std::unique_ptr<Resource>, size> pool;
                std::vector<u32> free_indices = {};
                u32 next_index = 0;
                u32 max_resources = size;
            };

            struct GPUResourceManager {
                GPUResourcePool<Buffer> buffer_pool = {};
                GPUResourcePool<Image> image_pool = {};
                GPUResourcePool<Sampler> sampler_pool = {};
            };
        }
    }
}