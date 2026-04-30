#include "VRI/resources/DescriptorSet.h"

TUnique<CDescriptorSet> VRICreateDescriptorSet(const VkDescriptorPool descriptorPool, const uint32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts) {
    const VkDescriptorSetAllocateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = descriptorSetCount,
        .pSetLayouts = setLayouts
   };
    TUnique<CDescriptorSet> descriptorSet;
    VK_CHECK(vkAllocateDescriptorSets(CVRI::get()->getDevice()->device, &createInfo, &descriptorSet->mDescriptorSet));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(descriptorSet);
}
