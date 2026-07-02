#pragma once

#include "cppns/memory/Unique.h"
#include "VRI/resources/VRIResources.h"

struct CSemaphore : SVRIResource {

    enum Flags {
        NONE = 0,
        SIGNALED = 0x00000001
    };

    CSemaphore() = default;

    [[nodiscard]] VkSemaphore& get() { return mSemaphore; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CSemaphore> VRICreateSemaphore(Flags);
    VkSemaphore mSemaphore = nullptr;
};

EXPORT TUnique<CSemaphore> VRICreateSemaphore(CSemaphore::Flags flags = CSemaphore::NONE);
