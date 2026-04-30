#include "VRI/resources/CommandPool.h"

TUnique<CCommandPool> VRICreateCommandPool(const uint32_t queueFamilyIndex, const CCommandPool::Flags flags) {
    const VkCommandPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkCommandPoolCreateFlags>(flags),
        .queueFamilyIndex = queueFamilyIndex
   };
    TUnique<CCommandPool> commandPool;
    VK_CHECK(vkCreateCommandPool(CVRI::get()->getDevice()->device, &createInfo, nullptr, &commandPool->mCommandPool));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(commandPool);
}