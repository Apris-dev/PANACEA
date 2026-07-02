#pragma once

#include "BindlessResources.h"
#include "basic/core/Class.h"
#include "basic/core/Object.h"
#include "resources/Sampler.h"
#include "cppns/container/Array.h"

class CVRISamplers final : public SObject {

    REGISTER_CLASS(CVRISamplers, SObject)

public:

    constexpr static uint32 gMaxSamplers = 64;
    constexpr static uint32 gSamplerBinding = 0;
    constexpr static TArray pools {
        SDescriptor::Pool{gSamplerBinding, SDescriptor::Pool::SAMPLER, gMaxSamplers},
    };

    EXPORT void init();

    EXPORT void destroy();

    no_discard TFrail<CSampler> getNearest() const { return mNearestSampler; }

    no_discard TFrail<CSampler> getLinear() const { return mLinearSampler; }

private:

    TUnique<CSampler> createSampler(std::string_view inName, const VkSamplerCreateInfo& inCreateInfo);

    TVector<std::string> samplerIndexes;
    SDescriptor descriptor;

    TUnique<CSampler> mNearestSampler = nullptr;

    TUnique<CSampler> mLinearSampler = nullptr;

};
