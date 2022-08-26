#include "pipeline.hpp"

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			Result<std::filesystem::path> RasterPipeline::try_get_path_to_file(const std::filesystem::path &path) {
				if (std::filesystem::exists(path)) {
					return { path };
				}
				std::filesystem::path potential_path;
				for (auto &root : std::filesystem::directory_iterator(std::filesystem::current_path())) {
					potential_path.clear();
					potential_path = root / path;
					if (std::filesystem::exists(potential_path)) {
						return { potential_path };
					}
				}
				std::string error_msg = {};
				error_msg += "could not find file :\"";
				error_msg += path.string();
				error_msg += "\"";
				return ResultErr{ .message = std::move(error_msg) };
			}

			Result<ShaderCode> RasterPipeline::try_read_file(const std::filesystem::path &path) {
				std::string code;
				std::ifstream in(path, std::ios::in | std::ios::binary);
				if (in) {
					in.seekg(0, std::ios::end);
					size_t size = in.tellg();
					if (size != -1) {
						code.resize(size);
						in.seekg(0, std::ios::beg);
						in.read(&code[0], size);
					} else {
						return ResultErr{ .message = "File is empty!" };
					}
				} else {
					return ResultErr{ .message = "File couldn't be found!" };
				}
				return ShaderCode{ .string = code };
			}

			Result<std::vector<u32>> RasterPipeline::compile_shader(const ShaderCode &code, VkShaderStageFlagBits shader_stage, const std::filesystem::path& path) {
				shaderc::PreprocessedSourceCompilationResult pre_result = compiler.PreprocessGlsl(code.string, shaderc_fragment_shader, "", options);

				shaderc_shader_kind shader_type = {};
				if (shader_stage == VK_SHADER_STAGE_VERTEX_BIT) {
					shader_type = shaderc_vertex_shader;
				} else if (shader_stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
					shader_type = shaderc_fragment_shader;
				} else if (shader_stage == VK_SHADER_STAGE_GEOMETRY_BIT) {
					shader_type = shaderc_geometry_shader;
				} else {
					return ResultErr{ .message = "Wrong shader type" };
				}

				shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(pre_result.begin(), shader_type, path.c_str(), options);

				if (pre_result.GetNumErrors() > 0) {
					return ResultErr{ .message = pre_result.GetErrorMessage() };
				}

				if (result.GetNumErrors() > 0) {
					return ResultErr{ .message = result.GetErrorMessage() };
				}

				return std::vector<uint32_t>(result.cbegin(), result.cend());
			}

			Result<std::vector<u32>> RasterPipeline::get_spirv(const ShaderInfo &shader_info, VkShaderStageFlagBits shader_stage) {
				std::vector<u32> spirv = {};
                std::filesystem::path path = {};
				if (shader_info.source.index() == 2) {
					const ShaderSPIRV &input_spirv = std::get<ShaderSPIRV>(shader_info.source);
					spirv.resize(input_spirv.size);
					for (usize i = 0; i < input_spirv.size; ++i) {
						spirv[i] = input_spirv.data[i];
					}
				} else {
					ShaderCode code = {};
					if (auto shader_source = std::get_if<ShaderFile>(&shader_info.source)) {
						auto ret = try_get_path_to_file(shader_source->path);
						if (ret.is_err()) {
							return ResultErr{ ret.message() };
						}
                        path = ret.value();
						auto code_ret = try_read_file(ret.value());
						if (code_ret.is_err()) {
							return ResultErr{ code_ret.message() };
						}
						code = code_ret.value();
					} else {
						code = std::get<ShaderCode>(shader_info.source);
					}

					auto ret = compile_shader(code, shader_stage, path);
					if (ret.is_err()) {
						return ResultErr{ ret.message() };
					}
					spirv = ret.value();
				}

				return spirv;
			}

			RasterPipeline::RasterPipeline(Device &_device, const RasterPipelineInfo &_info) : device{ _device }, info{ _info } {
				options.SetIncluder(std::make_unique<ShaderIncluder>());
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

				auto v_spirv_result = get_spirv(info.vertex_shader_info, VK_SHADER_STAGE_VERTEX_BIT);
				if (v_spirv_result.is_err()) {
					THROW(v_spirv_result.message());
				}
				std::vector<u32> v_spirv = v_spirv_result.value();

				auto f_spirv_result = get_spirv(info.fragment_shader_info, VK_SHADER_STAGE_FRAGMENT_BIT);
				if (f_spirv_result.is_err()) {
					THROW(f_spirv_result.message());
				}
				std::vector<u32> f_spirv = f_spirv_result.value();

				VkShaderModule v_vk_shader_module = {};
				VkShaderModule f_vk_shader_module = {};

				VkShaderModuleCreateInfo shader_module_vertex = {
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.pNext = nullptr,
					.codeSize = static_cast<u32>(v_spirv.size() * sizeof(u32)),
					.pCode = v_spirv.data(),
				};

				vkCreateShaderModule(device.device(), &shader_module_vertex, nullptr, &v_vk_shader_module);

				VkShaderModuleCreateInfo shader_module_fragment = {
					.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
					.pNext = nullptr,
					.codeSize = static_cast<u32>(f_spirv.size() * sizeof(u32)),
					.pCode = f_spirv.data(),
				};

				vkCreateShaderModule(device.device(), &shader_module_fragment, nullptr, &f_vk_shader_module);

				VkPipelineShaderStageCreateInfo vk_pipeline_shader_stage_create_infos[2] = {
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = {},
						.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
						.module = v_vk_shader_module,
						.pName = "main",
						.pSpecializationInfo = nullptr,
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.pNext = nullptr,
						.flags = {},
						.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = f_vk_shader_module,
						.pName = "main",
						.pSpecializationInfo = nullptr,
					},
				};

                VkPipelineVertexInputStateCreateInfo vk_vertex_input_state = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
					.pNext = nullptr,
					.vertexBindingDescriptionCount = static_cast<uint32_t>(info.vertex_input.binding.size()),
					.pVertexBindingDescriptions = info.vertex_input.binding.data(),
					.vertexAttributeDescriptionCount = static_cast<uint32_t>(info.vertex_input.attribute.size()),
					.pVertexAttributeDescriptions = info.vertex_input.attribute.data(),
				};
				constexpr VkPipelineInputAssemblyStateCreateInfo vk_input_assembly_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.pNext = nullptr,
					.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				};
				constexpr VkPipelineMultisampleStateCreateInfo vk_multisample_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.pNext = nullptr,
					.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
					.minSampleShading = 1.0f,
				};

				VkPipelineRasterizationStateCreateInfo vk_raster_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.pNext = nullptr,
					.polygonMode = *reinterpret_cast<VkPolygonMode const *>(&info.raster.polygon_mode),
					.cullMode = *reinterpret_cast<VkCullModeFlags const *>(&info.raster.face_culling),
					.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE,
					.lineWidth = 1.0f,
				};
				VkPipelineDepthStencilStateCreateInfo vk_depth_stencil_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.pNext = nullptr,
					.flags = {},
					.depthTestEnable = info.depth_test.enable_depth_test,
					.depthWriteEnable = info.depth_test.enable_depth_write,
					.depthCompareOp = static_cast<VkCompareOp>(info.depth_test.depth_test_compare_op),
					.depthBoundsTestEnable = VK_FALSE,
					.stencilTestEnable = VK_FALSE,
					.front = {},
					.back = {},
					.minDepthBounds = info.depth_test.min_depth_bounds,
					.maxDepthBounds = info.depth_test.max_depth_bounds,
				};

				std::vector<VkPipelineColorBlendAttachmentState> vk_pipeline_color_blend_attachment_blend_states = {};
				vk_pipeline_color_blend_attachment_blend_states.resize(info.color_attachments.size());
				for (usize i = 0; i < info.color_attachments.size(); ++i) {
					vk_pipeline_color_blend_attachment_blend_states[i] = *reinterpret_cast<VkPipelineColorBlendAttachmentState const *>(&info.color_attachments[i].blend);
				}

				std::vector<VkFormat> vk_pipeline_color_attachment_formats = {};
				vk_pipeline_color_attachment_formats.resize(info.color_attachments.size());
				for (usize i = 0; i < info.color_attachments.size(); ++i) {
					vk_pipeline_color_attachment_formats[i] = *reinterpret_cast<VkFormat const *>(&info.color_attachments[i].format);
				}

				VkPipelineColorBlendStateCreateInfo vk_color_blend_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.pNext = nullptr,
					.logicOpEnable = VK_FALSE,
					.logicOp = {},
					.attachmentCount = static_cast<u32>(info.color_attachments.size()),
					.pAttachments = vk_pipeline_color_blend_attachment_blend_states.data(),
					.blendConstants = { 1.0f, 1.0f, 1.0f, 1.0f },
				};

				constexpr VkViewport DEFAULT_VIEWPORT{ .width = 1, .height = 1 };
				constexpr VkRect2D DEFAULT_SCISSOR{ .extent = { 1, 1 } };

				VkPipelineViewportStateCreateInfo vk_viewport_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
					.pNext = nullptr,
					.viewportCount = 1,
					.pViewports = &DEFAULT_VIEWPORT,
					.scissorCount = 1,
					.pScissors = &DEFAULT_SCISSOR,
				};

				auto dynamic_state = std::array{
					VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
					VkDynamicState::VK_DYNAMIC_STATE_SCISSOR,
				};
				VkPipelineDynamicStateCreateInfo vk_dynamic_state{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
					.pNext = nullptr,
					.dynamicStateCount = static_cast<u32>(dynamic_state.size()),
					.pDynamicStates = dynamic_state.data(),
				};

				VkPipelineRenderingCreateInfo vk_pipeline_rendering{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
					.pNext = nullptr,
					.colorAttachmentCount = static_cast<u32>(info.color_attachments.size()),
					.pColorAttachmentFormats = vk_pipeline_color_attachment_formats.data(),
					.depthAttachmentFormat = static_cast<VkFormat>(info.depth_test.depth_attachment_format),
				};

				VkPushConstantRange vk_push_constant_range{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					.offset = 0,
					.size = info.pipeline_layout_info.push_constant_size
				};

				VkPipelineLayoutCreateInfo vk_pipeline_layout_create_info = {
					.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
					.pNext = 0,
					.flags = {},
					.setLayoutCount = static_cast<uint32_t>(info.pipeline_layout_info.vk_descriptor_set_layouts.size()),
					.pSetLayouts = info.pipeline_layout_info.vk_descriptor_set_layouts.data(),
					.pushConstantRangeCount = 1,
					.pPushConstantRanges = &vk_push_constant_range
				};

				if (vkCreatePipelineLayout(device.device(), &vk_pipeline_layout_create_info, nullptr, &vk_pipeline_layout) != VK_SUCCESS) {
					throw std::runtime_error("failed to create pipeline layout!");
				}

				VkGraphicsPipelineCreateInfo vk_graphics_pipeline_create_info{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.pNext = nullptr,
					.flags = {},
					.stageCount = 2,
					.pStages = vk_pipeline_shader_stage_create_infos,
					.pVertexInputState = &vk_vertex_input_state,
					.pInputAssemblyState = &vk_input_assembly_state,
					.pViewportState = &vk_viewport_state,
					.pRasterizationState = &vk_raster_state,
					.pMultisampleState = &vk_multisample_state,
					.pDepthStencilState = &vk_depth_stencil_state,
					.pColorBlendState = &vk_color_blend_state,
					.pDynamicState = &vk_dynamic_state,
					.layout = vk_pipeline_layout,
					.renderPass = info.vk_render_pass,
					.subpass = 0,
					.basePipelineHandle = VK_NULL_HANDLE,
					.basePipelineIndex = 0,
				};

				if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &vk_graphics_pipeline_create_info, nullptr, &vk_pipeline) != VK_SUCCESS) {
					THROW("failed to create graphics pipeline");
				}

				vkDestroyShaderModule(device.device(), v_vk_shader_module, nullptr);
				vkDestroyShaderModule(device.device(), f_vk_shader_module, nullptr);
			}

			RasterPipeline::~RasterPipeline() {
				vkDestroyPipeline(device.device(), vk_pipeline, nullptr);
				vkDestroyPipelineLayout(device.device(), vk_pipeline_layout, nullptr);
			}

			void RasterPipeline::bind(VkCommandBuffer commandBuffer) { vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline); }
		}
	}
}
