#include "VRI/resources/DescriptorSetLayout.h"

#include <VkBootstrap.h>
#include "VRI/VRI.h"

VkDescriptorSetLayoutCreateFlags convertToVkCreateFlags(const CDescriptorSetLayout::Flags inFlags) {
    switch (inFlags) {
    case CDescriptorSetLayout::Flags::UPDATE_AFTER_BIND_POOL:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    case CDescriptorSetLayout::Flags::PUSH_DESCRIPTOR:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT;
    case CDescriptorSetLayout::Flags::DESCRIPTOR_BUFFER:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    case CDescriptorSetLayout::Flags::EMBEDDED_IMMUTABLE_SAMPLERS:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_EMBEDDED_IMMUTABLE_SAMPLERS_BIT_EXT;
    case CDescriptorSetLayout::Flags::INDIRECT_BINDABLE:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_INDIRECT_BINDABLE_BIT_NV;
    case CDescriptorSetLayout::Flags::HOST_ONLY_POOL:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_HOST_ONLY_POOL_BIT_EXT;
    case CDescriptorSetLayout::Flags::PER_STAGE:
        return VK_DESCRIPTOR_SET_LAYOUT_CREATE_PER_STAGE_BIT_NV;
    default:
        return 0;
    }
}

TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(const uint32 bindingCount, const VkDescriptorSetLayoutBinding* bindings, const CDescriptorSetLayout::Flags flags) {
    const VkDescriptorSetLayoutCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = convertToVkCreateFlags(flags),
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