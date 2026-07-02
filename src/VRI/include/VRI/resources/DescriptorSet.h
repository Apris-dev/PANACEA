#pragma once

#include "cppns/memory/Unique.h"
#include "cppns/container/Vector.h"
#include "VRI/resources/VRIResources.h"

FORWARD_VK_TYPE(VkDescriptorSet);

struct CDescriptorSetLayout;
struct CDescriptorPool;

struct CDescriptorSet : SVRIResource {

    CDescriptorSet() = default;

    [[nodiscard]] VkDescriptorSet& get() { return mDescriptorSet; }

    /*void bind(const VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint, const VkPipelineLayout inPipelineLayout, const uint32 inFirstSet, const uint32 inDescriptorSetCount) const {
        vkCmdBindDescriptorSets(cmd, inBindPoint,inPipelineLayout, inFirstSet, inDescriptorSetCount, &mDescriptorSet, 0, nullptr);
    }*/

private:
    EXPORT friend TUnique<CDescriptorSet> VRICreateDescriptorSet(const TFrail<CDescriptorPool>&, const TVector<TFrail<CDescriptorSetLayout>>&, const VkDescriptorSetVariableDescriptorCountAllocateInfoEXT&);
    VkDescriptorSet mDescriptorSet = nullptr;
};

EXPORT TUnique<CDescriptorSet> VRICreateDescriptorSet(const TFrail<CDescriptorPool>& descriptorPool, const TVector<TFrail<CDescriptorSetLayout>>& inLayouts, const VkDescriptorSetVariableDescriptorCountAllocateInfoEXT& allocateInfo);
