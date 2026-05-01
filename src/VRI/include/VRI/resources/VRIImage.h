#pragma once

#include <vma/vk_mem_alloc.h>

#include "VRI/resources/VRIResources.h"

struct SVRIImage : SVRIResource {

    EXPORT SVRIImage(const std::string_view& inDebugName, VkExtent3D inExtent, VkFormat inFormat, VkImageUsageFlags inFlags = 0, VkImageAspectFlags inViewFlags = 0, uint32 inNumMips = 1);

    EXPORT virtual std::function<void()> getDestroyer() override;

    VkExtent3D getExtent() const { return mImageInfo.extent; }

    VkFormat getFormat() const { return mImageInfo.format; }

    bool isMipmapped() const { return mImageInfo.mipLevels > 1; }

    EXPORT void push(const TFrail<class CVRICommands>& cmd, const void* inData, const uint32& inSize);

    std::string mName = "Image";

    VkImage mImage = nullptr;
    VkImageCreateInfo mImageInfo;

    VkImageView mImageView = nullptr;
    VkImageViewCreateInfo mImageViewInfo;

    VmaAllocation mAllocation = nullptr;
    uint32 mBindlessAddress = -1;

    VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;

};