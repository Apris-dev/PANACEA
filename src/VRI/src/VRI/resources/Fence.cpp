#include "VRI/resources/Fence.h"

TUnique<CFence> VRICreateFence(const CFence::Flags flags) {
    const VkFenceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkFenceCreateFlags>(flags)
   };
    TUnique<CFence> fence;
    VK_CHECK(vkCreateFence(CVRI::get()->getDevice()->device, &createInfo, nullptr, &fence->mFence));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(fence);
}