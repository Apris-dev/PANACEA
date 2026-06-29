#pragma once

#include "sptr/Unique.h"
#include "VRI/resources/VRIResources.h"
#include "VRI/resources/VRIBuffer.h"

// Holds the resources needed for mesh rendering
struct SVRIMeshBuffer {

    SVRIMeshBuffer() = default;
    EXPORT SVRIMeshBuffer(size_t indicesSize, size_t verticesSize);

    TUnique<SVRIBuffer> indexBuffer = nullptr;
    TUnique<SVRIBuffer> vertexBuffer = nullptr;
};
