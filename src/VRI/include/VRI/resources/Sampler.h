#pragma once

#include "VRI/resources/VRIResources.h"

struct CSampler : SVRIResource {

    CSampler() = default;

    [[nodiscard]] VkSampler& get() { return mSampler; }

    virtual std::function<void()> getDestroyer() override {
        return [sampler = mSampler] {
            vkDestroySampler(CVRI::get()->getDevice()->device, sampler, nullptr);
        };
    }

private:
    EXPORT friend TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo&);
    VkSampler mSampler = nullptr;
};

EXPORT TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo& inCreateInfo);
