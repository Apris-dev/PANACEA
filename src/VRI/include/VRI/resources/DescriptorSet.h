#pragma once

#include "VRI/resources/VRIResources.h"

struct CDescriptorSet : SVRIResource {

    CDescriptorSet() = default;

    [[nodiscard]] VkDescriptorSet& get() { return mDescriptorSet; }

    void bind(const VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint, const VkPipelineLayout inPipelineLayout, const uint32 inFirstSet, const uint32 inDescriptorSetCount) const {
        vkCmdBindDescriptorSets(cmd, inBindPoint,inPipelineLayout, inFirstSet, inDescriptorSetCount, &mDescriptorSet, 0, nullptr);
    }

private:
    EXPORT friend TUnique<CDescriptorSet> VRICreateDescriptorSet(VkDescriptorPool, uint32, const VkDescriptorSetLayout*);
    VkDescriptorSet mDescriptorSet = nullptr;
};

EXPORT TUnique<CDescriptorSet> VRICreateDescriptorSet(VkDescriptorPool descriptorPool, uint32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts);
