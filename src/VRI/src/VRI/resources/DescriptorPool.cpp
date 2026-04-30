#include "VRI/resources/DescriptorPool.h"

TUnique<CDescriptorPool> VRICreateDescriptorPool(const uint32 maxSets, const uint32 poolSizeCount, const VkDescriptorPoolSize* poolSizes, const CDescriptorPool::Flags flags) {
    VkDescriptorPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkDescriptorPoolCreateFlags>(flags),
        .maxSets = maxSets,
        .poolSizeCount = poolSizeCount,
        .pPoolSizes = poolSizes
   };
    TUnique<CDescriptorPool> descriptorPool;
    VK_CHECK(vkCreateDescriptorPool(CVRI::get()->getDevice()->device, &createInfo, nullptr, &descriptorPool->mDescriptorPool));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(descriptorPool);
}