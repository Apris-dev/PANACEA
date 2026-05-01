#pragma once

#include "VRI/resources/VRIResources.h"

#include "sstl/Vector.h"

FORWARD_VK_TYPE(VkDescriptorPool);

struct CDescriptorPool : SVRIResource {

    enum Type {
        SAMPLER = 0,
        COMBINED_IMAGE_SAMPLER = 1,
        SAMPLED_IMAGE = 2,
        STORAGE_IMAGE = 3,
        UNIFORM_TEXEL_BUFFER = 4,
        STORAGE_TEXEL_BUFFER = 5,
        UNIFORM_BUFFER = 6,
        STORAGE_BUFFER = 7,
        UNIFORM_BUFFER_DYNAMIC = 8,
        STORAGE_BUFFER_DYNAMIC = 9,
        INPUT_ATTACHMENT = 10
    };

    enum class Flags : uint8 {
        NONE = 0,
        FREE_DESCRIPTOR_SET = 0x00000001,
        UPDATE_AFTER_BIND = 0x00000002,
        HOST_ONLY = 0x00000004,
        ALLOW_OVERALLOCATION_SETS = 0x00000008,
        ALLOW_OVERALLOCATION_POOLS = 0x00000010
    };
    
    struct PoolSize {
        Type type;
        uint32_t descriptorCount;
    };

    CDescriptorPool() = default;

    [[nodiscard]] VkDescriptorPool get() const { return mDescriptorPool; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32, const TVector<PoolSize>&, Flags);
    VkDescriptorPool mDescriptorPool = nullptr;
};

EXPORT TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32 maxSets, const TVector<CDescriptorPool::PoolSize>& poolSizes, CDescriptorPool::Flags flags = CDescriptorPool::Flags::NONE);
