#include "VRI/resources/CommandPool.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>

#include "VRI/VRI.h"

VkDescriptorPoolCreateFlags convertToVkCreateFlags(const CCommandPool::Flags inFlags) {
    switch (inFlags) {
    case CCommandPool::Flags::TRANSIENT:
        return VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    case CCommandPool::Flags::RESET_COMMAND_BUFFER:
        return VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    case CCommandPool::Flags::PROTECTED:
        return VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
    default:
        return 0;
    }
}

TUnique<CCommandPool> VRICreateCommandPool(const uint32_t queueFamilyIndex, const CCommandPool::Flags flags) {
    const VkCommandPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = convertToVkCreateFlags(flags),
        .queueFamilyIndex = queueFamilyIndex
   };
    TUnique<CCommandPool> commandPool;
    VK_CHECK(vkCreateCommandPool(CVRI::get()->getDevice()->device, &createInfo, nullptr, &commandPool->mCommandPool));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(commandPool);
}

std::function<void()> CCommandPool::getDestroyer() {
    return [commandPool = mCommandPool] {
        vkDestroyCommandPool(CVRI::get()->getDevice()->device, commandPool, nullptr);
    };
}