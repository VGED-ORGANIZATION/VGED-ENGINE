#pragma once

#include "vk_types.hpp"
#include "device.hpp"
#include "model.hpp"
#include "../core/debug.hpp"
#include "../core/result.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <variant>
#include <fstream>

#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
				shaderc_include_result *GetInclude(const char *requested_source, shaderc_include_type type, const char *requesting_source, size_t include_depth) override {
					// BS
					std::string msg = std::string(requesting_source);
					msg += std::to_string(type);
					msg += static_cast<char>(include_depth);

					const std::string name = std::string(requested_source);
					const std::string contents = readFile(name);

					auto container = new std::array<std::string, 2>;
					(*container)[0] = name;
					(*container)[1] = contents;

					auto data = new shaderc_include_result;

					data->user_data = container;

					data->source_name = (*container)[0].data();
					data->source_name_length = (*container)[0].size();

					data->content = (*container)[1].data();
					data->content_length = (*container)[1].size();

					return data;
				};

				void ReleaseInclude(shaderc_include_result *data) override {
					delete static_cast<std::array<std::string, 2> *>(data->user_data);
					delete data;
				};

				static std::string readFile(const std::string &filepath) {
					std::string code;
					std::ifstream in(filepath, std::ios::in | std::ios::binary);
					if (in) {
						in.seekg(0, std::ios::end);
						u32 size = static_cast<u32>(in.tellg());
						if (size != 0) {
							code.resize(size);
							in.seekg(0, std::ios::beg);
							in.read(&code[0], size);
						} else {
							THROW("File is empty!");
						}
					} else {
						THROW("Couldn't open a file!");
					}
					return code;
				}
			};

			struct ShaderFile {
				std::filesystem::path path;
			};

			struct ShaderCode {
				std::string string;
			};

			struct ShaderSPIRV {
				u32 *data;
				usize size;
			};

			using ShaderSource = std::variant<ShaderFile, ShaderCode, ShaderSPIRV>;

			struct ShaderInfo {
				ShaderSource source;
			};

			struct PipelineLayoutInfo {
				u32 push_constant_size = 0;
				std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts = {};
			};

            struct VertexInput {
                std::vector<VkVertexInputBindingDescription> binding = Model::Vertex::getBindingDescriptions();
                std::vector<VkVertexInputAttributeDescription> attribute = Model::Vertex::getAttributeDescriptions();
            };

			struct RasterPipelineInfo {
				ShaderInfo vertex_shader_info = {};
				ShaderInfo fragment_shader_info = {};
				ShaderInfo geometry_shader_info = {}; // shouldn't be used
				std::vector<RenderAttachment> color_attachments = {};
				DepthTestInfo depth_test = {};
				RasterizerInfo raster = {};
				VkRenderPass vk_render_pass = {};
				PipelineLayoutInfo pipeline_layout_info = {};
                VertexInput vertex_input = {};
			};

			class RasterPipeline {
			public:
				RasterPipeline(Device &_device, const RasterPipelineInfo &_info);
				~RasterPipeline();

				RasterPipeline(const RasterPipeline &) = delete;
				RasterPipeline &operator=(const RasterPipeline &) = delete;

				void bind(VkCommandBuffer commandBuffer);

                VkPipeline& pipeline() { return vk_pipeline; }
                VkPipelineLayout& pipeline_layout() { return vk_pipeline_layout; }

			private:
				Result<std::vector<u32>> get_spirv(const ShaderInfo &shader_info, VkShaderStageFlagBits shader_stage);
				Result<std::filesystem::path> try_get_path_to_file(const std::filesystem::path &path);
				Result<ShaderCode> try_read_file(const std::filesystem::path &path);
				Result<std::vector<u32>> compile_shader(const ShaderCode &code, VkShaderStageFlagBits shader_stage, const std::filesystem::path& path);

				shaderc::Compiler compiler;
				shaderc::CompileOptions options;

				Device &device;
				RasterPipelineInfo info;
				VkPipeline vk_pipeline = {};
				VkPipelineLayout vk_pipeline_layout = {};
			};
		}
	}
}
