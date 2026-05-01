#pragma once

#include "VRI/resources/VRIResources.h"

struct CDescriptorSetLayout : SVRIResource {

    enum Flags {
        NONE = 0,
        UPDATE_AFTER_BIND_POOL = 0x00000002,
        PUSH_DESCRIPTOR = 0x00000001,
        DESCRIPTOR_BUFFER = 0x00000010,
        EMBEDDED_IMMUTABLE_SAMPLERS = 0x00000020,
        INDIRECT_BINDABLE = 0x00000080,
        HOST_ONLY_POOL = 0x00000004,
        PER_STAGE = 0x00000040
    };

    CDescriptorSetLayout() = default;

    [[nodiscard]] VkDescriptorSetLayout& get() { return mDescriptorSetLayout; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32, const VkDescriptorSetLayoutBinding*, Flags);
    VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
};

EXPORT TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32 bindingCount, const VkDescriptorSetLayoutBinding* bindings, CDescriptorSetLayout::Flags flags = CDescriptorSetLayout::NONE);
