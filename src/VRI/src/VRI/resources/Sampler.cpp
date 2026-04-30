#include "VRI/resources/Sampler.h"

TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo& inCreateInfo) {
    TUnique<CSampler> sampler;
    VK_CHECK(vkCreateSampler(CVRI::get()->getDevice()->device, &inCreateInfo, nullptr, &sampler->mSampler));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(sampler);
}
