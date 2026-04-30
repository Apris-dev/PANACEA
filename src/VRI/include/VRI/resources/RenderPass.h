#pragma once

#include "VRI/resources/VRIResources.h"

struct CRenderPass : SVRIResource {

    enum Flags {
        NONE = 0,
        TRANSFORM = 0x00000002,
        PER_LAYER_FRAGMENT_DENSITY = 0x00000004
    };

    CRenderPass() = default;

    [[nodiscard]] VkRenderPass& get() { return mRenderPass; }

    virtual std::function<void()> getDestroyer() override {
        return [renderPass = mRenderPass] {
            vkDestroyRenderPass(CVRI::get()->getDevice()->device, renderPass, nullptr);
        };
    }

private:
    EXPORT friend TUnique<CRenderPass> VRICreateRenderPass(
        uint32,
        const VkAttachmentDescription*,
        uint32,
        const VkSubpassDescription*,
        uint32,
        const VkSubpassDependency*,
        Flags
    );
    VkRenderPass mRenderPass = nullptr;
};

EXPORT TUnique<CRenderPass> VRICreateRenderPass(
    uint32 attachmentCount,
    const VkAttachmentDescription* attachments,
    uint32 subpassCount,
    const VkSubpassDescription* subpasses,
    uint32 dependencyCount,
    const VkSubpassDependency* dependencies,
    CRenderPass::Flags flags = CRenderPass::NONE
);
