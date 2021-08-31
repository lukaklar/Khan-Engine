#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkancore.h"
#include "graphics/hal/vulkan/vulkanshader.hpp"
#include "graphics/hal/vulkan/vulkanphysicalrenderpassmanager.hpp"

namespace Khan
{
	KH_FORCE_INLINE void ExtractShaderData(std::vector<VkDescriptorSetLayoutBinding> descriptorSetindings[ResourceBindFrequency_Count], VkPipelineShaderStageCreateInfo& shaderStageInfo, uint32_t& stageCount, const Shader* shader)
	{
		if (shader != nullptr)
		{
			const VulkanShader* vulkanShader = reinterpret_cast<const VulkanShader*>(shader);

			shaderStageInfo = vulkanShader->GetShaderInfo();
			++stageCount;

			const SpvReflectShaderModule& reflection = vulkanShader->GetReflection();

			for (uint32_t i = 0; i < reflection.descriptor_binding_count; ++i)
			{
				const SpvReflectDescriptorBinding& reflectBinding = reflection.descriptor_bindings[i];

				VkDescriptorSetLayoutBinding descriptorSetBinding;
				descriptorSetBinding.binding = reflectBinding.binding;
				descriptorSetBinding.descriptorType = static_cast<VkDescriptorType>(reflectBinding.descriptor_type);
				descriptorSetBinding.descriptorCount = reflectBinding.count;
				descriptorSetBinding.stageFlags = vulkanShader->GetShaderInfo().stage;
				descriptorSetBinding.pImmutableSamplers = nullptr;

				descriptorSetindings[reflectBinding.set].emplace_back(descriptorSetBinding);
			}
		}	
	}

	KH_FORCE_INLINE static void TranslateVertexInputState(VkPipelineVertexInputStateCreateInfo& info, const VertexInputState& state)
	{
		static const VkFormat s_vkVertexBufferFormats[] =
		{
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32G32_SINT,
			VK_FORMAT_R32G32B32_SINT,
			VK_FORMAT_R32G32B32A32_SINT,
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_R32G32B32_UINT,
			VK_FORMAT_R32G32B32A32_UINT
		};

		// TODO: This is a serious bug when you start creating pipelines from multiple threads
		static VkVertexInputBindingDescription vertexBindingDescriptions[K_MAX_VERTEX_DATA_STREAMS];
		static VkVertexInputAttributeDescription vertexAttributeDescriptions[K_MAX_VERTEX_DATA_STREAMS * K_MAX_STREAM_ELEMENTS];
		uint32_t vertexBindingDescriptionCount = 0;
		uint32_t vertexAttributeDescriptionCount = 0;

		uint32_t location = 0;
		for (uint32_t i = 0; i < K_MAX_VERTEX_DATA_STREAMS; ++i)
		{
			if (state.GetActiveStreamMask() & (1 << i))
			{
				const VertexInputState::StreamDescriptor& streamDesc = state.GetStreamDescriptors()[i];

				VkVertexInputBindingDescription& bindingDesc = vertexBindingDescriptions[vertexBindingDescriptionCount++];
				bindingDesc.binding = i;
				bindingDesc.stride = streamDesc.GetStride();
				bindingDesc.inputRate = state.GetInstancedStreamMask() & (1 << i) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;

				for (uint32_t j = 0; j < streamDesc.GetNumElements(); ++j)
				{
					const VertexInputState::StreamDescriptor::StreamElement& streamElem = streamDesc.GetStreamElements()[j];

					VkVertexInputAttributeDescription& attributeDesc = vertexAttributeDescriptions[vertexAttributeDescriptionCount++];
					attributeDesc.location = location++;
					attributeDesc.binding = i;
					attributeDesc.format = s_vkVertexBufferFormats[static_cast<uint32_t>(streamElem.GetType())];
					attributeDesc.offset = streamElem.GetOffset();
				}
			}
		}

		info.pVertexBindingDescriptions = vertexBindingDescriptions;
		info.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
		info.pVertexAttributeDescriptions = vertexAttributeDescriptions;
		info.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
	}

	KH_FORCE_INLINE static void TranslateInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo& info, PrimitiveTopology topology, IndexStripCut indexStrpCut)
	{
		static const VkPrimitiveTopology  s_vkPrimitiveTopology[] =
		{
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
		};

		info.topology = s_vkPrimitiveTopology[static_cast<uint32_t>(topology)];
		info.primitiveRestartEnable = indexStrpCut != IndexStripCut::Disabled;
	}

	KH_FORCE_INLINE static void TranslateTesselationState(VkPipelineTessellationStateCreateInfo& info, PrimitiveTopology topology)
	{
		uint32_t controlPoints = static_cast<uint32_t>(topology) - static_cast<uint32_t>(PrimitiveTopology::PatchList1CP) + 1;
		info.patchControlPoints = controlPoints > 0 ? controlPoints : 0;
	}

	KH_FORCE_INLINE static void TranslateDepthStencilState(VkPipelineDepthStencilStateCreateInfo& info, const DepthStencilState& state)
	{
		static const VkStencilOp s_VkStencilOp[] =
		{
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_ZERO,
			VK_STENCIL_OP_REPLACE,
			VK_STENCIL_OP_INCREMENT_AND_CLAMP,
			VK_STENCIL_OP_DECREMENT_AND_CLAMP,
			VK_STENCIL_OP_INVERT,
			VK_STENCIL_OP_INCREMENT_AND_WRAP,
			VK_STENCIL_OP_DECREMENT_AND_WRAP
		};

		static const VkCompareOp s_VkComparisonFunc[] =
		{
			VK_COMPARE_OP_NEVER,
			VK_COMPARE_OP_LESS,
			VK_COMPARE_OP_EQUAL,
			VK_COMPARE_OP_LESS_OR_EQUAL,
			VK_COMPARE_OP_GREATER,
			VK_COMPARE_OP_NOT_EQUAL,
			VK_COMPARE_OP_GREATER_OR_EQUAL,
			VK_COMPARE_OP_ALWAYS
		};

		info.depthTestEnable = state.m_DepthMode.m_DepthTestEnabled ? VK_TRUE : VK_FALSE;
		info.depthWriteEnable = state.m_DepthMode.m_DepthWriteEnabled ? VK_TRUE : VK_FALSE;
		info.depthCompareOp = s_VkComparisonFunc[static_cast<uint32_t>(state.m_DepthMode.m_DepthFunc)];
		info.depthBoundsTestEnable = state.m_DepthMode.m_DepthBoundsTestEnabled ? VK_TRUE : VK_FALSE;
		info.stencilTestEnable = state.m_StencilMode.m_StencilTestEnabled ? VK_TRUE : VK_FALSE;
		info.front.failOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_FrontFace.m_StencilFailOp)];
		info.front.passOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_FrontFace.m_StencilPassOp)];
		info.front.depthFailOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_FrontFace.m_DepthFailOp)];
		info.front.compareMask = s_VkComparisonFunc[static_cast<uint32_t>(state.m_StencilMode.m_FrontFace.m_StencilFunc)];
		info.front.compareMask = state.m_StencilMode.m_ReadMask;
		info.front.writeMask = state.m_StencilMode.m_WriteMask;
		info.front.reference = state.m_StencilMode.m_StencilReference;
		info.back.failOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_BackFace.m_StencilFailOp)];
		info.back.passOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_BackFace.m_StencilPassOp)];
		info.back.depthFailOp = s_VkStencilOp[static_cast<uint32_t>(state.m_StencilMode.m_BackFace.m_DepthFailOp)];
		info.back.compareMask = s_VkComparisonFunc[static_cast<uint32_t>(state.m_StencilMode.m_BackFace.m_StencilFunc)];
		info.back.compareMask = state.m_StencilMode.m_ReadMask;
		info.back.writeMask = state.m_StencilMode.m_WriteMask;
		info.back.reference = state.m_StencilMode.m_StencilReference;
		info.minDepthBounds = state.m_DepthMode.m_MinDepthBound;
		info.maxDepthBounds = state.m_DepthMode.m_MaxDepthBound;
	}

	KH_FORCE_INLINE static void TranslateRasterizerState(VkPipelineRasterizationStateCreateInfo& info, const RasterizerState& state)
	{
		// should be front first and back later but these are switched to fix a bug for vulkan
		static const VkCullModeFlagBits s_vkCullMode[] =
		{
			VK_CULL_MODE_NONE,
			VK_CULL_MODE_BACK_BIT,
			VK_CULL_MODE_FRONT_BIT
		};

		// TODO: Enable this in RasterizerState
		info.depthClampEnable = VK_FALSE;
		info.rasterizerDiscardEnable = VK_FALSE;
		info.polygonMode = state.m_WireframeEnabled ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		info.cullMode = s_vkCullMode[static_cast<uint32_t>(state.m_CullMode)];
		info.frontFace = state.m_FrontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		info.depthBiasEnable = state.m_DepthBiasEnabled ? VK_TRUE : VK_FALSE;
		info.depthBiasConstantFactor = static_cast<float>(state.m_DepthBias);
		info.depthBiasClamp = state.m_DepthBiasClamp;
		info.depthBiasSlopeFactor = state.m_SlopeScaledDepthBias;
		info.lineWidth = 1.0f;
	}

	inline static void TranslateBlendState(VkPipelineColorBlendStateCreateInfo& info, const BlendState& state, uint8_t renderTargetCount, const float blendConstants[4])
	{
		static const VkBlendFactor s_vkBlendFactor[] =
		{
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_SRC_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
			VK_BLEND_FACTOR_DST_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_FACTOR_DST_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
			VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
			VK_BLEND_FACTOR_SRC1_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
			VK_BLEND_FACTOR_SRC1_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,
			VK_BLEND_FACTOR_CONSTANT_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		};

		static const VkBlendOp s_vkBlendOperation[] =
		{
			VK_BLEND_OP_ADD,
			VK_BLEND_OP_SUBTRACT,
			VK_BLEND_OP_REVERSE_SUBTRACT,
			VK_BLEND_OP_MIN,
			VK_BLEND_OP_MAX
		};

		static const VkLogicOp s_vkLogicOperator[] =
		{
			VK_LOGIC_OP_NO_OP,
			VK_LOGIC_OP_CLEAR,
			VK_LOGIC_OP_SET,
			VK_LOGIC_OP_COPY,
			VK_LOGIC_OP_COPY_INVERTED,
			VK_LOGIC_OP_INVERT,
			VK_LOGIC_OP_AND,
			VK_LOGIC_OP_NAND,
			VK_LOGIC_OP_AND_REVERSE,
			VK_LOGIC_OP_AND_INVERTED,
			VK_LOGIC_OP_OR,
			VK_LOGIC_OP_NOR,
			VK_LOGIC_OP_OR_REVERSE,
			VK_LOGIC_OP_OR_INVERTED,
			VK_LOGIC_OP_XOR,
			VK_LOGIC_OP_EQUIVALENT
		};

		static VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[K_MAX_RENDER_TARGETS];

		info.logicOpEnable = VK_FALSE;
		info.logicOp = VK_LOGIC_OP_NO_OP;
		info.attachmentCount = renderTargetCount;
		info.pAttachments = colorBlendAttachmentStates;
		std::memcpy(info.blendConstants, blendConstants, sizeof(blendConstants));

		for (uint32_t i = 0; i < renderTargetCount; ++i)
		{
			VkPipelineColorBlendAttachmentState& blendState = colorBlendAttachmentStates[i];
			blendState.blendEnable = state.m_BlendModes[i].m_BlendEnabled ? VK_TRUE : VK_FALSE;
			blendState.srcColorBlendFactor = s_vkBlendFactor[static_cast<uint32_t>(state.m_BlendModes[i].m_SrcColorFactor)];
			blendState.dstColorBlendFactor = s_vkBlendFactor[static_cast<uint32_t>(state.m_BlendModes[i].m_DstColorFactor)];
			blendState.colorBlendOp = s_vkBlendOperation[static_cast<uint32_t>(state.m_BlendModes[i].m_ColorBlendOp)];
			blendState.srcAlphaBlendFactor = s_vkBlendFactor[static_cast<uint32_t>(state.m_BlendModes[i].m_SrcAlphaFactor)];
			blendState.dstAlphaBlendFactor = s_vkBlendFactor[static_cast<uint32_t>(state.m_BlendModes[i].m_DstAlphaFactor)];
			blendState.alphaBlendOp = s_vkBlendOperation[static_cast<uint32_t>(state.m_BlendModes[i].m_AlphaBlendOp)];
			blendState.colorWriteMask = state.m_BlendModes[i].m_ColorWriteMask;
		}
	}

	KH_FORCE_INLINE VulkanPipelineState* VulkanPipelineStateManager::FindOrCreateGraphicsPipelineState(VkDevice device, const GraphicsPipelineDescription& desc)
	{
		auto it = m_GraphicsPipelineCache.find(desc);
		if (it != m_GraphicsPipelineCache.end())
		{
			return it->second;
		}
		return CreateGraphicsPipelineState(device, desc);
	}

	KH_FORCE_INLINE VulkanPipelineState* VulkanPipelineStateManager::FindOrCreateComputePipelineState(VkDevice device, const ComputePipelineDescription& desc)
	{
		auto it = m_ComputePipelineCache.find(desc);
		if (it != m_ComputePipelineCache.end())
		{
			return it->second;
		}
		return CreateComputePipelineState(device, desc);
	}

	KH_FORCE_INLINE VulkanPipelineState* VulkanPipelineStateManager::CreateGraphicsPipelineState(VkDevice device, const GraphicsPipelineDescription& desc)
	{
		VulkanPipelineState* pipelineState = new VulkanPipelineState();

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetBindings[ResourceBindFrequency_Count];
		VkPipelineShaderStageCreateInfo shaderStageInfos[5];
		uint32_t stageCount = 0;

		ExtractShaderData(descriptorSetBindings, shaderStageInfos[stageCount], stageCount, desc.m_VertexShader);
		ExtractShaderData(descriptorSetBindings, shaderStageInfos[stageCount], stageCount, desc.m_PixelShader);
		ExtractShaderData(descriptorSetBindings, shaderStageInfos[stageCount], stageCount, desc.m_HullShader);
		ExtractShaderData(descriptorSetBindings, shaderStageInfos[stageCount], stageCount, desc.m_DomainShader);
		ExtractShaderData(descriptorSetBindings, shaderStageInfos[stageCount], stageCount, desc.m_GeometryShader);

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		for (uint32_t i = 0; i < ResourceBindFrequency_Count; ++i)
		{
			descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetBindings[i].size());
			descriptorSetLayoutInfo.pBindings = descriptorSetBindings[i].data();
			VK_ASSERT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &pipelineState->m_DescriptorSetLayout[i]), "[VULKAN] Failed to create descriptor set layout.");
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutInfo.setLayoutCount = ResourceBindFrequency_Count;
		pipelineLayoutInfo.pSetLayouts = pipelineState->m_DescriptorSetLayout;
		VK_ASSERT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineState->m_PipelineLayout), "[VULKAN] Failed to create pipeline layout");

		VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.stageCount = stageCount;
		pipelineInfo.pStages = shaderStageInfos;

		VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		TranslateVertexInputState(vertexInputState, desc.m_VertexInputState);
		pipelineInfo.pVertexInputState = &vertexInputState;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		TranslateInputAssemblyState(inputAssemblyState, desc.m_PrimitiveTopology, desc.m_IndexStripCutValue);
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;

		VkPipelineTessellationStateCreateInfo tessellationState;
		if (desc.m_HullShader || desc.m_DomainShader)
		{
			tessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
			TranslateTesselationState(tessellationState, desc.m_PrimitiveTopology);
			pipelineInfo.pTessellationState = &tessellationState;
		}

		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		pipelineInfo.pViewportState = &viewportState;

		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		TranslateRasterizerState(rasterizationState, desc.m_RasterizerState);
		pipelineInfo.pRasterizationState = &rasterizationState;

		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.alphaToCoverageEnable = desc.m_BlendState.m_AlphaToCoverageEnabled ? VK_TRUE : VK_FALSE;
		pipelineInfo.pMultisampleState = &multisampleState;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		TranslateDepthStencilState(depthStencilState, desc.m_DepthStencilState);
		pipelineInfo.pDepthStencilState = &depthStencilState;

		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		TranslateBlendState(colorBlendState, desc.m_BlendState, desc.m_PhysicalRenderPass->GetDesc().m_RenderTargetCount, desc.m_BlendFactor);
		pipelineInfo.pColorBlendState = &colorBlendState;

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = _countof(dynamicStates);
		dynamicState.pDynamicStates = dynamicStates;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = pipelineState->m_PipelineLayout;
		pipelineInfo.renderPass = reinterpret_cast<const VulkanPhysicalRenderPass*>(desc.m_PhysicalRenderPass)->VulkanRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_ASSERT(vkCreateGraphicsPipelines(device, m_PipelineCache, 1, &pipelineInfo, nullptr, &pipelineState->m_Pipeline), "[VULKAN] Failed to create graphics pipeline.");

		m_GraphicsPipelineCache.emplace(desc, pipelineState);

		return pipelineState;
	}

	KH_FORCE_INLINE VulkanPipelineState* VulkanPipelineStateManager::CreateComputePipelineState(VkDevice device, const ComputePipelineDescription& desc)
	{
		VulkanPipelineState* pipelineState = new VulkanPipelineState();

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetBindings[ResourceBindFrequency_Count];
		VkPipelineShaderStageCreateInfo shaderStageInfo;
		uint32_t stageCount = 0;

		ExtractShaderData(descriptorSetBindings, shaderStageInfo, stageCount, desc.m_ComputeShader);

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		for (uint32_t i = 0; i < ResourceBindFrequency_Count; ++i)
		{
			descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetBindings[i].size());
			descriptorSetLayoutInfo.pBindings = descriptorSetBindings[i].data();
			VK_ASSERT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, &pipelineState->m_DescriptorSetLayout[i]), "[VULKAN] Failed to create descriptor set layout.");
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutInfo.setLayoutCount = ResourceBindFrequency_Count;
		pipelineLayoutInfo.pSetLayouts = pipelineState->m_DescriptorSetLayout;
		VK_ASSERT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineState->m_PipelineLayout), "[VULKAN] Failed to create pipeline layout");

		VkComputePipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
		pipelineInfo.stage = shaderStageInfo;
		pipelineInfo.layout = pipelineState->m_PipelineLayout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_ASSERT(vkCreateComputePipelines(device, m_PipelineCache, 1, &pipelineInfo, nullptr, &pipelineState->m_Pipeline), "[VULKAN] Failed to create compute pipeline.");

		m_ComputePipelineCache.emplace(desc, pipelineState);

		return pipelineState;
	}
}

#endif // KH_GFXAPI_VULKAN