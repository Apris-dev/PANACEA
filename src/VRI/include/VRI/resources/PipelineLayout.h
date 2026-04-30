#pragma once

#include "VRI/resources/VRIResources.h"

struct CPipelineLayout : SVRIResource {

    enum Flags {
        NONE = 0,
        INDEPENDENT_SETS = 0x00000002
    };

    CPipelineLayout() = default;

    [[nodiscard]] VkPipelineLayout get() const { return mPipelineLayout; }

    virtual std::function<void()> getDestroyer() override {
        return [pipelineLayout = mPipelineLayout] {
            vkDestroyPipelineLayout(CVRI::get()->getDevice()->device, pipelineLayout, nullptr);
        };
    }

private:
    EXPORT friend TUnique<CPipelineLayout> VRICreatePipelineLayout(uint32, const VkDescriptorSetLayout*, uint32, const VkPushConstantRange*, Flags);
    VkPipelineLayout mPipelineLayout = nullptr;
};

EXPORT TUnique<CPipelineLayout> VRICreatePipelineLayout(uint32 setLayoutCount, const VkDescriptorSetLayout* setLayouts, uint32 pushConstantRangeCount, const VkPushConstantRange* pushConstantRages, CPipelineLayout::Flags flags = CPipelineLayout::NONE);
