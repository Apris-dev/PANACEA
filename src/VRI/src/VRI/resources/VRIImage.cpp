#include "VRI/resources/VRIImage.h"

#include "VRI/BindlessResources.h"
#include "VRI/VRICommands.h"
#include "VRI/resources/DescriptorSet.h"
#include "VRI/resources/VRIBuffer.h"

SVRIImage::SVRIImage(const std::string_view& inDebugName, const VkExtent3D inExtent, const VkFormat inFormat, const VkImageUsageFlags inFlags, const VkImageAspectFlags inViewFlags, const uint32 inNumMips)
: mName(inDebugName) {

	mImageInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
	   .pNext = nullptr,

	   .imageType = VK_IMAGE_TYPE_2D,

	   .format = inFormat,
	   .extent = inExtent,

	   .mipLevels = inNumMips,
	   .arrayLayers = 1,

	   //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
	   .samples = VK_SAMPLE_COUNT_1_BIT,

	   //optimal tiling, which means the image is stored on the best gpu format
	   .tiling = VK_IMAGE_TILING_OPTIMAL,
	   .usage = inFlags,
	   .sharingMode = VK_SHARING_MODE_EXCLUSIVE
   };

	//for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo imageAllocationInfo = {
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
		.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	};

	vmaCreateImage(CVRI::get()->getVkAllocator(), &mImageInfo, &imageAllocationInfo, &mImage, &mAllocation, nullptr);

	// Build an image-view for the draw image to use for rendering
	mImageViewInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,

		.image = mImage,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = inFormat,
		.subresourceRange = {
			.aspectMask = inViewFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	mImageViewInfo.subresourceRange.levelCount = mImageInfo.mipLevels;

	VK_CHECK(vkCreateImageView(CVRI::get()->getDevice()->device, &mImageViewInfo, nullptr, &mImageView));

	// Update descriptors with new image
	if ((inFlags & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) { //TODO: VK_IMAGE_USAGE_SAMPLED_BIT is not a permanent solution
		// Set and increment current texture address
		static uint32 gCurrentTextureAddress = 0;
		mBindlessAddress = gCurrentTextureAddress;
		gCurrentTextureAddress++;

		const auto imageDescriptorInfo = VkDescriptorImageInfo{
			.imageView = mImageView,
			.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
		};

		const auto writeSet = VkWriteDescriptorSet{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = CBindlessResources::getBindlessDescriptorSet()->get(),
			.dstBinding = gTextureBinding,
			.dstArrayElement = mBindlessAddress,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImageInfo = &imageDescriptorInfo,
		};
		vkUpdateDescriptorSets(CVRI::get()->getDevice()->device, 1, &writeSet, 0, nullptr);
	}
}

void generateMipmaps(const TFrail<CVRICommands>& cmd, const TFrail<SVRIImage>& image) {
	int mipLevels = int(std::floor(std::log2(std::max(image->getExtent().width, image->getExtent().height)))) + 1;
	VkExtent2D extent = {image->getExtent().width, image->getExtent().height};
	for (int mip = 0; mip < mipLevels; mip++) {

        VkExtent2D halfSize = extent;
        halfSize.width /= 2;
        halfSize.height /= 2;

        VkImageMemoryBarrier2 imageBarrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr };

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		imageBarrier.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = static_cast<uint32>(mip),
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS
		};

        imageBarrier.image = image->mImage;

        VkDependencyInfo depInfo { .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = nullptr };
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &imageBarrier;

		cmd->pipelineBarrier2(depInfo);

        if (mip < mipLevels - 1) {
            VkImageBlit2 blitRegion { .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

            blitRegion.srcOffsets[1].x = static_cast<int32>(extent.width);
            blitRegion.srcOffsets[1].y = static_cast<int32>(extent.height);
            blitRegion.srcOffsets[1].z = 1;

            blitRegion.dstOffsets[1].x = static_cast<int32>(halfSize.width);
            blitRegion.dstOffsets[1].y = static_cast<int32>(halfSize.height);
            blitRegion.dstOffsets[1].z = 1;

            blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.srcSubresource.baseArrayLayer = 0;
            blitRegion.srcSubresource.layerCount = 1;
            blitRegion.srcSubresource.mipLevel = mip;

            blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.dstSubresource.baseArrayLayer = 0;
            blitRegion.dstSubresource.layerCount = 1;
            blitRegion.dstSubresource.mipLevel = mip + 1;

            VkBlitImageInfo2 blitInfo {.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
            blitInfo.dstImage = image->mImage;
            blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            blitInfo.srcImage = image->mImage;
            blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            blitInfo.filter = VK_FILTER_LINEAR;
            blitInfo.regionCount = 1;
            blitInfo.pRegions = &blitRegion;

        	cmd->blitImage2(blitInfo);

            extent = halfSize;
        }
    }

    // transition all mip levels into the final read_only layout
	cmd->transitionImage(image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void SVRIImage::push(const TFrail<CVRICommands>& cmd, const void* inData, const uint32& inSize) {
	//size_t data_size = inExtent.depth * inExtent.width * inExtent.height * 4;

	// Upload buffer is not needed outside of this function
	// TODO: Some way of doing an upload buffer generically
	//SStagingBuffer uploadBuffer{allocator, mName, inSize}; //TODO: was CPU_TO_GPU, test if errors
	TUnique<SVRIBuffer> uploadBuffer{inSize, VMA_MEMORY_USAGE_CPU_ONLY, VK_BUFFER_USAGE_TRANSFER_SRC_BIT};
	uploadBuffer->push(inData, inSize);
	//memcpy(uploadBuffer.get(allocator)->getMappedData(), inData, inSize);

	cmd->transitionImage(this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	const VkBufferImageCopy copyRegion {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageExtent = getExtent()
	};

	// copy the buffer into the image
	cmd->copyBufferToImage(uploadBuffer, this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, copyRegion);

	if (isMipmapped()) {
		generateMipmaps(cmd, this);
	} else {
		cmd->transitionImage(this, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

std::function<void()> SVRIImage::getDestroyer() {
	return [image = mImage, allocation = mAllocation, imageView = mImageView] {
		vmaDestroyImage(CVRI::get()->getVkAllocator(), image, allocation);
		vkDestroyImageView(CVRI::get()->getDevice()->device, imageView, nullptr);
	};
}