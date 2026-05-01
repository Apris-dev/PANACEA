#include "VRI/resources/Sampler.h"

#include <VkBootstrap.h>

#include "VRI/VRI.h"

TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo& inCreateInfo) {
    TUnique<CSampler> sampler;
    VK_CHECK(vkCreateSampler(CVRI::get()->getDevice()->device, &inCreateInfo, nullptr, &sampler->mSampler));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(sampler);
}

std::function<void()> CSampler::getDestroyer() {
    return [sampler = mSampler] {
        vkDestroySampler(CVRI::get()->getDevice()->device, sampler, nullptr);
    };
}