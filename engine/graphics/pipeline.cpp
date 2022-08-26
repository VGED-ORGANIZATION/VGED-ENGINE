#include "pipeline.hpp"

#include "core/result.hpp"
#include "model.hpp"
#include "shaderc/shaderc.h"

// std
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "../core/debug.hpp"

namespace VGED {
	namespace Engine {
		inline namespace Graphics {
			Pipeline::Pipeline(Device &_device, const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo) : device{ _device } { createGraphicsPipeline(vertFilepath, fragFilepath, configInfo); }

			Pipeline::~Pipeline() {
				vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
				vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
				vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
			}

			std::vector<char> Pipeline::readFile(const std::string &filepath) {
				std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

				if (!file.is_open()) {
					throw std::runtime_error("failed to open file: " + filepath);
				}

				size_t fileSize = static_cast<size_t>(file.tellg());
				std::vector<char> buffer(fileSize);

				file.seekg(0);
				file.read(buffer.data(), fileSize);

				file.close();
				return buffer;
			}

			void Pipeline::createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo) {
				assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
				assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

				auto vertCode = readFile(vertFilepath);
				auto fragCode = readFile(fragFilepath);

				createShaderModule(vertCode, &vertShaderModule);
				createShaderModule(fragCode, &fragShaderModule);

				VkPipelineShaderStageCreateInfo shaderStages[2];
				shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStages[0].module = vertShaderModule;
				shaderStages[0].pName = "main";
				shaderStages[0].flags = 0;
				shaderStages[0].pNext = nullptr;
				shaderStages[0].pSpecializationInfo = nullptr;
				shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStages[1].module = fragShaderModule;
				shaderStages[1].pName = "main";
				shaderStages[1].flags = 0;
				shaderStages[1].pNext = nullptr;
				shaderStages[1].pSpecializationInfo = nullptr;

				auto &bindingDescriptions = configInfo.bindingDescriptions;
				auto &attributeDescriptions = configInfo.attributeDescriptions;
				VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
				vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

				VkGraphicsPipelineCreateInfo pipelineInfo{};
				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;
				pipelineInfo.pStages = shaderStages;
				pipelineInfo.pVertexInputState = &vertexInputInfo;
				pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
				pipelineInfo.pViewportState = &configInfo.viewportInfo;
				pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
				pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
				pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
				pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
				pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

				pipelineInfo.layout = configInfo.pipelineLayout;
				pipelineInfo.renderPass = configInfo.renderPass;
				pipelineInfo.subpass = configInfo.subpass;

				pipelineInfo.basePipelineIndex = -1;
				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

				if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
					throw std::runtime_error("failed to create graphics pipeline");
				}
			}

			void Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = code.size();
				createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

				if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
					throw std::runtime_error("failed to create shader module");
				}
			}

			void Pipeline::bind(VkCommandBuffer commandBuffer) { vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); }

			void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo &configInfo) {
				configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

				configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				configInfo.viewportInfo.viewportCount = 1;
				configInfo.viewportInfo.pViewports = nullptr;
				configInfo.viewportInfo.scissorCount = 1;
				configInfo.viewportInfo.pScissors = nullptr;

				configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
				configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
				configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
				configInfo.rasterizationInfo.lineWidth = 1.0f;
				configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
				configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
				configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
				configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
				configInfo.rasterizationInfo.depthBiasClamp = 0.0f; // Optional
				configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

				configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
				configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				configInfo.multisampleInfo.minSampleShading = 1.0f; // Optional
				configInfo.multisampleInfo.pSampleMask = nullptr; // Optional
				configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
				configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

				configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
				configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
				configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
				configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
				configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
				configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
				configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

				configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
				configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
				configInfo.colorBlendInfo.attachmentCount = 1;
				configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
				configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
				configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
				configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
				configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

				configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
				configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
				configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
				configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
				configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
				configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
				configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
				configInfo.depthStencilInfo.front = {}; // Optional
				configInfo.depthStencilInfo.back = {}; // Optional

				configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
				configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
				configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
				configInfo.dynamicStateInfo.flags = 0;

				configInfo.bindingDescriptions = VGED::Engine::Model::Vertex::getBindingDescriptions();
				configInfo.attributeDescriptions = VGED::Engine::Model::Vertex::getAttributeDescriptions();
			}

			void Pipeline::enableAlphaBlending(PipelineConfigInfo &configInfo) {
				configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
				configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
				configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			}

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
