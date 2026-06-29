#pragma once

#include <vma/vk_mem_alloc.h>
#include "VRI/resources/VRIResources.h"

struct SVRIBuffer : SVRIResource {

    EXPORT SVRIBuffer(size_t inBufferSize, VmaMemoryUsage inMemoryUsage, VkBufferUsageFlags inBufferUsage = 0);

    EXPORT void makeGlobal(); //TODO: alternate way of doing this
    EXPORT void updateGlobal() const; // TODO: not global but instead 'dynamic' (address should be separate?)

    EXPORT virtual std::function<void()> getDestroyer() override;

    no_discard EXPORT void* getMappedData() const;

    EXPORT void mapData(void** data) const;

    EXPORT void unMapData() const;

    bool isAllocated() const {
        return allocation != nullptr;
    }

    template <typename... TArgs>
    void push(const void* src, const size_t size, TArgs... args) {
        memcpy(getMappedData(), src, size);
        push(size, args...);
    }

    VkBuffer buffer = nullptr;

    VmaAllocation allocation = nullptr;
    VmaAllocationInfo info = {};
    size_t size;

    uint32 mBindlessAddress = 0;

private:

    template <typename... TArgs>
    void push(const size_t offset = 0) {}

    template <typename... TArgs>
    void push(const size_t offset, const void* src, const size_t size, TArgs... args) {
        memcpy(static_cast<char*>(getMappedData()) + offset, src, size);
        push(offset + size, args...);
    }
};