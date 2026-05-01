#include "VRI/resources/Semaphore.h"

#include <VkBootstrap.h>

#include "VRI/VRI.h"

TUnique<CSemaphore> VRICreateSemaphore(const CSemaphore::Flags flags) {
    VkSemaphoreCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkSemaphoreCreateFlags>(flags)
   };
    TUnique<CSemaphore> semaphore;
    VK_CHECK(vkCreateSemaphore(CVRI::get()->getDevice()->device, &createInfo, nullptr, &semaphore->mSemaphore));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(semaphore);
}

std::function<void()> CSemaphore::getDestroyer() {
    return [semaphore = mSemaphore] {
        vkDestroySemaphore(CVRI::get()->getDevice()->device, semaphore, nullptr);
    };
}