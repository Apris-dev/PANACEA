#pragma once

#include "sptr/Unique.h"
#include "VRI/resources/VRIResources.h"

struct CFence : SVRIResource {

    enum Flags {
        NONE = 0,
        SIGNALED = 0x00000001
    };

    CFence() = default;

    [[nodiscard]] VkFence& get() { return mFence; }

    virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<CFence> VRICreateFence(Flags);
    VkFence mFence = nullptr;
};

EXPORT TUnique<CFence> VRICreateFence(CFence::Flags flags = CFence::NONE);
