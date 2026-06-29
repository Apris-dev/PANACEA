#include "VRI/resources/PipelineLayout.h"

#include <VkBootstrap.h>

#include "VRI/VRI.h"

TUnique<CPipelineLayout> VRICreatePipelineLayout(const TVector<VkDescriptorSetLayout>& setLayouts, const TVector<VkPushConstantRange>& pushConstantRanges, CPipelineLayout::Flags flags) {
    const VkPipelineLayoutCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkPipelineLayoutCreateFlags>(flags),
        .setLayoutCount = static_cast<uint32>(setLayouts.getSize()),
        .pSetLayouts = setLayouts.data(),
        .pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.getSize()),
        .pPushConstantRanges = pushConstantRanges.data()
   };
    TUnique<CPipelineLayout> pipelineLayout;
    VK_CHECK(vkCreatePipelineLayout(CVRI::get()->getDevice()->device, &createInfo, nullptr, &pipelineLayout->mPipelineLayout));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(pipelineLayout);
}

std::function<void()> CPipelineLayout::getDestroyer() {
    return [pipelineLayout = mPipelineLayout] {
        vkDestroyPipelineLayout(CVRI::get()->getDevice()->device, pipelineLayout, nullptr);
    };
}