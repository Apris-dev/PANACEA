#pragma once

#include "VRI/resources/VRIResources.h"

//The Layout is a Template, the Pool is the Memory, and the Set is the Instance.
struct CDescriptorSetLayout : SVRIResource {

    enum class Flags : uint8 {
        NONE = 0,
        UPDATE_AFTER_BIND_POOL = 0x00000002,
        PUSH_DESCRIPTOR = 0x00000001,
        DESCRIPTOR_BUFFER = 0x00000010,
        EMBEDDED_IMMUTABLE_SAMPLERS = 0x00000020,
        INDIRECT_BINDABLE = 0x00000080,
        HOST_ONLY_POOL = 0x00000004,
        PER_STAGE = 0x00000040
    };

    /*struct Binding {
        uint32 binding;
        VkDescriptorType descriptorType;
        uint32 descriptorCount;
        VkShaderStageFlags stageFlags;
        const VkSampler* pImmutableSamplers;
    };*/

    CDescriptorSetLayout() = default;

    [[nodiscard]] VkDescriptorSetLayout& get() { return mDescriptorSetLayout; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32, const VkDescriptorSetLayoutBinding*, const VkDescriptorSetLayoutBindingFlagsCreateInfo&, Flags);
    VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
};

EXPORT TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32 bindingCount, const VkDescriptorSetLayoutBinding* bindings, const VkDescriptorSetLayoutBindingFlagsCreateInfo& bindingInfo, CDescriptorSetLayout::Flags flags = CDescriptorSetLayout::Flags::NONE);
