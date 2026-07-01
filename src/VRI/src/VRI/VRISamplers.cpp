#include "VRI/VRISamplers.h"

#include <VkBootstrap.h>

#include "VRI/VRI.h"
#include "VRI/resources/Sampler.h"

TUnique<CSampler> CVRISamplers::createSampler(const std::string_view inName, const VkSamplerCreateInfo& inCreateInfo) {
    TUnique<CSampler> sampler = VRICreateSampler(inCreateInfo);

    const auto imageDescriptorInfo = VkDescriptorImageInfo{
        .sampler = sampler->get()
    };

    // Add samplers to tracking
    const uint32 currentSampler = samplerIndexes.push(std::string(inName));

    const auto writeSet = VkWriteDescriptorSet{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor.mDescriptorSet,
        .dstBinding = gSamplerBinding,
        .dstArrayElement = currentSampler,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &imageDescriptorInfo,
    };

    const auto sets = {writeSet};

    vkUpdateDescriptorSets(CVRI::get()->getDevice()->device, static_cast<uint32>(sets.size()), sets.begin(), 0, nullptr);

    return std::move(sampler);
}

void CVRISamplers::init() {
    descriptor.init(pools,
        SDescriptor::CAN_HAVE_EMPTY_SLOTS
    );

    VkSamplerCreateInfo samplerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT
    };

    samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

    mNearestSampler = createSampler("SAMPLER_NEAREST", samplerCreateInfo);

    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    mLinearSampler = createSampler("SAMPLER_LINEAR", samplerCreateInfo);
}

void CVRISamplers::destroy() {
    mLinearSampler.destroy();
    mNearestSampler.destroy();
    descriptor.destroy();
}