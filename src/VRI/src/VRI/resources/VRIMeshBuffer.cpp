#include "VRI/resources/VRIMeshBuffer.h"

SVRIMeshBuffer::SVRIMeshBuffer(const size_t indicesSize, const size_t verticesSize) {
    indexBuffer = TUnique<SVRIBuffer>{indicesSize, VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
    vertexBuffer = TUnique<SVRIBuffer>{verticesSize, VMA_MEMORY_USAGE_GPU_ONLY, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
}
