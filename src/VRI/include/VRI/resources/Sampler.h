#pragma once

#include "sptr/Unique.h"
#include "VRI/resources/VRIResources.h"

struct CSampler : SVRIResource {

    CSampler() = default;

    [[nodiscard]] VkSampler& get() { return mSampler; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo&);
    VkSampler mSampler = nullptr;
};

EXPORT TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo& inCreateInfo);
