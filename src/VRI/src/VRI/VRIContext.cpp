#include "VRI/VRIContext.h"

// TODO: fences themselves might be able to track resources
TUnique<CFence> CVRIContext::createFence(const VkFenceCreateFlags inFlags) {
    VkFenceCreateInfo fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
       .pNext = nullptr,
       .flags = inFlags
   };
    TUnique<CFence> fence{fenceCreateInfo};
    resources.push(fence);
    return std::move(fence);
}