#include "VRI/resources/DescriptorPool.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

#include "VRI/VRI.h"

VkDescriptorType convertToVkType(const CDescriptorPool::Type inType) {
    switch (inType) {
    case CDescriptorPool::SAMPLER:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case CDescriptorPool::COMBINED_IMAGE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case CDescriptorPool::SAMPLED_IMAGE:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case CDescriptorPool::STORAGE_IMAGE:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case CDescriptorPool::UNIFORM_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case CDescriptorPool::STORAGE_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case CDescriptorPool::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case CDescriptorPool::STORAGE_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case CDescriptorPool::UNIFORM_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case CDescriptorPool::STORAGE_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case CDescriptorPool::INPUT_ATTACHMENT:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    default:
        return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

VkDescriptorPoolCreateFlags convertToVkCreateFlags(const CDescriptorPool::Flags inFlags) {
    switch (inFlags) {
    case CDescriptorPool::Flags::FREE_DESCRIPTOR_SET:
        return VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    case CDescriptorPool::Flags::UPDATE_AFTER_BIND:
        return VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT;
    case CDescriptorPool::Flags::HOST_ONLY:
        return VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT;
    case CDescriptorPool::Flags::ALLOW_OVERALLOCATION_SETS:
        return VK_DESCRIPTOR_POOL_CREATE_ALLOW_OVERALLOCATION_SETS_BIT_NV;
    case CDescriptorPool::Flags::ALLOW_OVERALLOCATION_POOLS:
        return VK_DESCRIPTOR_POOL_CREATE_ALLOW_OVERALLOCATION_POOLS_BIT_NV;
    default:
        return 0;
    }
}

TUnique<CDescriptorPool> VRICreateDescriptorPool(const uint32 maxSets, const TVector<CDescriptorPool::PoolSize>& poolSizes, const CDescriptorPool::Flags flags) {
    TVector<VkDescriptorPoolSize> descriptorPoolSizes;
    descriptorPoolSizes.reserve(poolSizes.getSize());

    for (const auto& [type, descriptorCount] : poolSizes) {
        descriptorPoolSizes.push({
            convertToVkType(type),
            descriptorCount
        });
    }

    const VkDescriptorPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkDescriptorPoolCreateFlags>(flags),
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32>(descriptorPoolSizes.getSize()),
        .pPoolSizes = descriptorPoolSizes.data(),
   };
    TUnique<CDescriptorPool> descriptorPool;
    VK_CHECK(vkCreateDescriptorPool(CVRI::get()->getDevice()->device, &createInfo, nullptr, &descriptorPool->mDescriptorPool));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(descriptorPool);
}

std::function<void()> CDescriptorPool::getDestroyer() {
    return [descriptorPool = mDescriptorPool] {
        vkDestroyDescriptorPool(CVRI::get()->getDevice()->device, descriptorPool, nullptr);
    };
}