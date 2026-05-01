#include "VRI/resources/DescriptorSetLayout.h"

#include <VkBootstrap.h>
#include "VRI/VRI.h"

TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(const uint32 bindingCount, const VkDescriptorSetLayoutBinding* bindings, const CDescriptorSetLayout::Flags flags) {
    VkDescriptorSetLayoutCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkDescriptorSetLayoutCreateFlags>(flags),
        .bindingCount = bindingCount,
        .pBindings = bindings
   };
    TUnique<CDescriptorSetLayout> descriptorSetLayout;
    VK_CHECK(vkCreateDescriptorSetLayout(CVRI::get()->getDevice()->device, &createInfo, nullptr, &descriptorSetLayout->mDescriptorSetLayout));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(descriptorSetLayout);
}

std::function<void()> CDescriptorSetLayout::getDestroyer() {
    return [descriptorSetLayout = mDescriptorSetLayout] {
        vkDestroyDescriptorSetLayout(CVRI::get()->getDevice()->device, descriptorSetLayout, nullptr);
    };
}