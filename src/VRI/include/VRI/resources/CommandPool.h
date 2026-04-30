#pragma once

#include "VRI/resources/VRIResources.h"

struct CCommandPool : SVRIResource {

    enum Flags {
        NONE = 0,
        TRANSIENT = 0x00000001,
        RESET_COMMAND_BUFFER = 0x00000002,
        PROTECTED = 0x00000004
    };

    CCommandPool() = default;

    [[nodiscard]] VkCommandPool get() const { return mCommandPool; }

    virtual std::function<void()> getDestroyer() override {
        return [commandPool = mCommandPool] {
            vkDestroyCommandPool(CVRI::get()->getDevice()->device, commandPool, nullptr);
        };
    }

private:
    EXPORT friend TUnique<CCommandPool> VRICreateCommandPool(uint32, Flags);
    VkCommandPool mCommandPool = nullptr;
};

EXPORT TUnique<CCommandPool> VRICreateCommandPool(uint32 queueFamilyIndex, CCommandPool::Flags flags = CCommandPool::NONE);
