#include "VRI/resources/DescriptorSet.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

#include "VRI/VRI.h"
#include "VRI/resources/DescriptorPool.h"
#include "VRI/resources/DescriptorSetLayout.h"

TUnique<CDescriptorSet> VRICreateDescriptorSet(const TFrail<CDescriptorPool>& descriptorPool, const TVector<TFrail<CDescriptorSetLayout>>& inLayouts) {
    TVector<VkDescriptorSetLayout> setLayouts;
    setLayouts.reserve(inLayouts.getSize());

    for (const auto& layout : inLayouts) {
        setLayouts.push(layout->get());
    }

    const VkDescriptorSetAllocateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool->get(),
        .descriptorSetCount = static_cast<uint32>(setLayouts.getSize()),
        .pSetLayouts = setLayouts.data()
   };
    TUnique<CDescriptorSet> descriptorSet;
    VK_CHECK(vkAllocateDescriptorSets(CVRI::get()->getDevice()->device, &createInfo, &descriptorSet->mDescriptorSet));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(descriptorSet);
}
