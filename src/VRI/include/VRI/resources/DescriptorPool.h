#pragma once

#include "VRI/resources/VRIResources.h"

struct CDescriptorPool : SVRIResource {

    enum Flags {
        NONE = 0,
        FREE_DESCRIPTOR_SET = 0x00000001,
        UPDATE_AFTER_BIND = 0x00000002,
        HOST_ONLY = 0x00000004,
        ALLOW_OVERALLOCATION_SETS = 0x00000008,
        ALLOW_OVERALLOCATION_POOLS = 0x00000010
    };

    CDescriptorPool() = default;

    [[nodiscard]] VkDescriptorPool get() const { return mDescriptorPool; }

    virtual std::function<void()> getDestroyer() override {
        return [descriptorPool = mDescriptorPool] {
            vkDestroyDescriptorPool(CVRI::get()->getDevice()->device, descriptorPool, nullptr);
        };
    }

private:
    EXPORT friend TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32, uint32, const VkDescriptorPoolSize*, Flags);
    VkDescriptorPool mDescriptorPool = nullptr;
};

EXPORT TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32 maxSets, uint32 poolSizeCount, const VkDescriptorPoolSize* poolSizes, CDescriptorPool::Flags flags = CDescriptorPool::NONE);
