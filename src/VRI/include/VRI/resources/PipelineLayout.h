#pragma once

#include "sptr/Unique.h"
#include "sstl/Vector.h"
#include "VRI/resources/VRIResources.h"

struct CPipelineLayout : SVRIResource {

    enum Flags {
        NONE = 0,
        INDEPENDENT_SETS = 0x00000002
    };

    CPipelineLayout() = default;

    [[nodiscard]] VkPipelineLayout get() const { return mPipelineLayout; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CPipelineLayout> VRICreatePipelineLayout(const TVector<VkDescriptorSetLayout>&, const TVector<VkPushConstantRange>&, Flags);
    VkPipelineLayout mPipelineLayout = nullptr;
};

EXPORT TUnique<CPipelineLayout> VRICreatePipelineLayout(const TVector<VkDescriptorSetLayout>& setLayouts, const TVector<VkPushConstantRange>& pushConstantRanges, CPipelineLayout::Flags flags = CPipelineLayout::NONE);
