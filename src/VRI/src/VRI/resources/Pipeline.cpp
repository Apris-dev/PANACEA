#include "VRI/resources/Pipeline.h"

#include <VkBootstrap.h>

#include "VRI/VRI.h"
#include "VRI/resources/PipelineLayout.h"

TUnique<SPipeline> VRICreatePipeline(const SPipelineCreateInfo& inCreateInfo, CVertexAttributeArchive& inAttributes, const TUnique<CPipelineLayout>& inLayout) {

	TUnique<SPipeline> pipeline;
	pipeline->mLayout = inLayout;

	// Make viewport state from our stored viewport and scissor.
	// At the moment we won't support multiple viewports or scissors
	//TODO: multiple viewports, although supported on many GPUs, is not all of them, so it should have a alternative
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineColorBlendAttachmentState colorBlendAttachment {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	auto setBlending =[&]{
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	};

	switch (inCreateInfo.mBlendMode) {
		case EBlendMode::NONE:
			colorBlendAttachment.blendEnable = VK_FALSE;
			break;
		case EBlendMode::ADDITIVE:
			setBlending();
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;
		case EBlendMode::ALPHA_BLEND:
			setBlending();
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			break;
	}

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	const VkPipelineVertexInputStateCreateInfo vertexInputInfo = inAttributes.get();

	std::vector state {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	/*if (inCreateInfo.mLineWidth != 1.f) {
		//state.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
	}*/

	VkPipelineDynamicStateCreateInfo dynamicInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = (uint32)state.size(),
		.pDynamicStates = state.data()
	};

	VkPipelineRenderingCreateInfo renderingCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &inCreateInfo.mColorFormat,
		.depthAttachmentFormat = inCreateInfo.mDepthFormat
	};

	std::vector shaderStages = {
		VkPipelineShaderStageCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = inCreateInfo.vertexModule,
			.pName = "main"
		}
	};

	// Fragment shader isn't always necessary, but vertex is
	if (inCreateInfo.fragmentModule) {
		shaderStages.push_back(VkPipelineShaderStageCreateInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = inCreateInfo.fragmentModule,
			.pName = "main"
		});
	}

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = inCreateInfo.mTopology,
		.primitiveRestartEnable = VK_FALSE
	};

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = inCreateInfo.mPolygonMode,
		.cullMode = inCreateInfo.mCullMode,
		.frontFace = inCreateInfo.mFrontFace,
		.lineWidth = inCreateInfo.mLineWidth
	};

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	};

	if (inCreateInfo.mUseMultisampling) {
		//TODO: multisampling
	} else {
		multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCreateInfo.minSampleShading = 1.0f;
		multisampleCreateInfo.pSampleMask = nullptr;
		multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleCreateInfo.alphaToOneEnable = VK_FALSE;
	}

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
		.minDepthBounds = 0.f,
		.maxDepthBounds = 1.f
	};

	switch (inCreateInfo.mDepthTestMode) {
		case EDepthTestMode::NORMAL:
			depthStencilCreateInfo.depthTestEnable = VK_TRUE;
			depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
			depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
			break;
		case EDepthTestMode::BEHIND:
			depthStencilCreateInfo.depthTestEnable = VK_FALSE;
			depthStencilCreateInfo.depthWriteEnable = VK_FALSE;
			depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
			break;
		case EDepthTestMode::FRONT:
			depthStencilCreateInfo.depthTestEnable = VK_FALSE;
			depthStencilCreateInfo.depthWriteEnable = VK_FALSE;
			depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
			break;
	}

	// Build the actual pipeline
	// We now use all the info structs we have been writing into into this one
	// To create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &renderingCreateInfo,
		.stageCount = (uint32)shaderStages.size(),
		.pStages = shaderStages.data(),
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssemblyCreateInfo,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationCreateInfo,
		.pMultisampleState = &multisampleCreateInfo,
		.pDepthStencilState = &depthStencilCreateInfo,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicInfo,
		.layout = inLayout->get()
	};

	if (vkCreateGraphicsPipelines(CVRI::get()->getDevice()->device, VK_NULL_HANDLE, 1, &pipelineInfo,nullptr, &pipeline->mPipeline) != VK_SUCCESS) {
		msgs("Failed to create pipeline!");
	}

	return std::move(pipeline);
}

VkPipelineLayout SPipeline::getLayout() const {
	return mLayout->get();
}

void SPipeline::bind(const VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint) const {
	vkCmdBindPipeline(cmd, inBindPoint, mPipeline);
}

std::function<void()> SPipeline::getDestroyer() {
	return [pipeline = mPipeline] {
		vkDestroyPipeline(CVRI::get()->getDevice()->device, pipeline, nullptr);
	};
}
