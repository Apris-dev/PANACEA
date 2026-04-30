#include "VRI/resources/PipelineLayout.h"

TUnique<CPipelineLayout> VRICreatePipelineLayout(const uint32 setLayoutCount, const VkDescriptorSetLayout* setLayouts, const uint32 pushConstantRangeCount, const VkPushConstantRange* pushConstantRages, const CPipelineLayout::Flags flags) {
    const VkPipelineLayoutCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkPipelineLayoutCreateFlags>(flags),
        .setLayoutCount = setLayoutCount,
        .pSetLayouts = setLayouts,
        .pushConstantRangeCount = pushConstantRangeCount,
        .pPushConstantRanges = pushConstantRages
   };
    TUnique<CPipelineLayout> pipelineLayout;
    VK_CHECK(vkCreatePipelineLayout(CVRI::get()->getDevice()->device, &createInfo, nullptr, &pipelineLayout->mPipelineLayout));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(pipelineLayout);
}
