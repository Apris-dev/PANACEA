#include "VRI/VRIAllocator.h"

CVRIAllocator::CVRIAllocator() {
    const VmaAllocatorCreateInfo allocatorInfo {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = CVRI::get()->getDevice()->physical_device,
        .device = CVRI::get()->getDevice()->device,
        .instance = CVRI::get()->getInstance()->instance
    };

    VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_Allocator));
}

void CVRIAllocator::destroy2() {

    for (size_t overlap = 0; overlap < CVRI::get()->getSwapchain()->m_Buffering.getFrameOverlap(); ++overlap) {
        popDeferredQueue(overlap);
    }

    for (auto itr = m_Resources.rbegin(); itr != m_Resources.rend(); ++itr) {
        const TFrail<SVRIResource>& resource = *itr;
        resource->getDestroyer()();
    }
    m_Resources.clear();

    vmaDestroyAllocator(m_Allocator);
}