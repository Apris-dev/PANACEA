#pragma once

#include "cppns/container/Vector.h"
#include "VRI/resources/VRIResources.h"

enum class EShaderStage : uint8 {
    VERTEX,
    FRAGMENT,
    COMPUTE
};

struct SShader : SVRIResource {

    EXPORT SShader(const char* inFilePath);

    EXPORT virtual std::function<void()> getDestroyer() override;

    std::string mFileName = "";
    VkShaderModule mModule = nullptr;
    EShaderStage mStage = EShaderStage::VERTEX;
    std::string mShaderCode = "";
    TVector<uint32> mCompiledShader{};

private:

    uint32 compile();

    bool loadShader(const char* inFileName, uint32 Hash);

    bool saveShader(const char* inFileName, uint32 Hash) const;
};
