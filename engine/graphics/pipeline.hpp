#pragma once

#include "device.hpp"

#include <variant>
#include <filesystem>

namespace VGED {
    namespace Engine {
        inline namespace Graphics {
            struct ShaderFile {
                std::filesystem::path path;
            };

            struct ShaderCode {
                std::string string;
            };

            struct ShaderSPIRV {
                u32* data;
                usize size;
            };

            using ShaderSource = std::variant<ShaderFile, ShaderCode, ShaderSPIRV>;

            struct ShaderInfo {
                ShaderSource source;
                std::string entry_point = {"main"};
            };

            struct DepthTestInfo {
                Format depth_attachment_format = Format::UNDEFINED;
                bool enable_depth_test = false;
                bool enable_depth_write = false;
                CompareOp depth_test_compare_op = CompareOp::LESS_OR_EQUAL;
                f32 min_depth_bounds = 0.0f;
                f32 max_depth_bounds = 1.0f;
            };

            struct RasterizerInfo {
                PolygonMode polygon_mode = PolygonMode::FILL;
                FaceCullFlags face_culling = FaceCullFlagBits::NONE;
                bool depth_clamp_enable = false;
                bool rasterizer_discard_enable = false;
                bool depth_bias_enable = false;
                f32 depth_bias_constant_factor = 0.0f;
                f32 depth_bias_clamp = 0.0f;
                f32 depth_bias_slope_factor = 0.0f;
                f32 line_width = 1.0f;
            };

            struct RenderAttachment {
                Format format = {};
                BlendInfo blend = {};
            };

            struct RasterizationPipelineInfo {
                ShaderInfo vertex_shader_info = {};
                ShaderInfo fragment_shader_info = {};
                ShaderInfo geometry_Shader_info = {};
                std::vector<RenderAttachment> color_attachments = {};
                DepthTestInfo depth_test = {};
                RasterizerInfo raster = {};
                u32 push_constant_size = {};
            };

            class RasterizationPipeline {
                RasterizationPipeline(const RasterizationPipelineInfo& info);
                ~RasterizationPipeline();
            };
        }
    }
}