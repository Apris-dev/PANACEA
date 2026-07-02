#include "VRI/resources/VRIResources.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "VkBootstrap.h"
#include "cppns/archive/FileArchive.h"
#include "VRI/VRIAllocator.h"

#include "VRI/resources/VRIBuffer.h"
#include "VRI/resources/VRIImage.h"

SVRIResource::SVRIResource() {
	asts(gIsVRIInitialized, "Cannot create VRI Resource before VRI Initialization!");
	msgs("Attempted to allocate a resource.");
	CVRI::get()->getAllocator()->pushResource(this);
}

void SVRIResource::release() {
	if (gIsVRIInitialized) {
		msgs("Attempted to release a resource.");
		CVRI::get()->getAllocator()->releaseResource(this);
	}
	delete this;
}

VkRenderingAttachmentInfo SRenderAttachment::get(const SVRIImage* inImage) const {
	VkRenderingAttachmentInfo info {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.loadOp = mLoadOp,
		.storeOp = mStoreOp
	};

	if (inImage) {
		info.imageView = inImage->mImageView;
		info.imageLayout = inImage->mLayout;
	}

	switch (mType) {
		case EAttachmentType::COLOR:
			info.clearValue = {
				.color = {
					.float32 = {
						mClearValue[0],
						mClearValue[1],
						mClearValue[2],
						mClearValue[3]
					}
				}
			};
			break;
		case EAttachmentType::DEPTH:
		case EAttachmentType::STENCIL:
			info.clearValue = {
				.depthStencil = {
					.depth = mClearValue[0],
					.stencil = static_cast<uint32>(mClearValue[1])
				}
			};
			break;
	}

	return info;
}

void* SVRIBuffer::getMappedData() const {
	return allocation->GetMappedData();
}