#pragma once

#include <vulkan/vulkan_core.h>

#include "basic/core/Common.h"

#define FORWARD_VK_TYPE(x) \
	typedef struct x##_T* x

inline const char* string_VkResult(VkResult result) {
    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";

        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";

        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";

        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";

        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";

        // Add more as needed for newer extensions

        default: return "VK_UNKNOWN_ERROR";
    }
}

#define VK_CHECK(call) \
	if (auto vkResult = call; vkResult != VK_SUCCESS) { \
		errs("{} Failed. Vulkan Error {}", #call, string_VkResult(vkResult)); \
	}

struct SVRIResource {

	EXPORT SVRIResource();
	virtual ~SVRIResource() = default;

	// No copying vulkan resources, since most need to be deleted, and if they are copied, could be deleted more than once
	SVRIResource(const SVRIResource&) = delete;
	SVRIResource& operator=(const SVRIResource&) = delete;

	virtual std::function<void()> getDestroyer() { return {}; }

	EXPORT void release();
};

enum class EAttachmentType : uint8 {
	COLOR,
	DEPTH,
	STENCIL
};

struct SRenderAttachment {
	VkAttachmentLoadOp mLoadOp = VK_ATTACHMENT_LOAD_OP_NONE;
	VkAttachmentStoreOp mStoreOp = VK_ATTACHMENT_STORE_OP_NONE;
	EAttachmentType mType = EAttachmentType::COLOR;
	Vector4f mClearValue = Vector4f{0.f};

	constexpr static SRenderAttachment defaultColor() {
		return {
			.mLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.mStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
			.mType = EAttachmentType::COLOR
		};
	}

	constexpr static SRenderAttachment defaultDepth() {
		return {
			.mLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.mStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
			.mType = EAttachmentType::DEPTH
		};
	}

	constexpr static SRenderAttachment defaultStencil() {
		return {
			.mLoadOp = VK_ATTACHMENT_LOAD_OP_NONE,
			.mStoreOp = VK_ATTACHMENT_STORE_OP_NONE,
			.mType = EAttachmentType::STENCIL
		};
	}

	EXPORT VkRenderingAttachmentInfo get(const struct SVRIImage* inImage) const;

	bool operator==(const SRenderAttachment& inOther) const {
		return mLoadOp == inOther.mLoadOp && mStoreOp == inOther.mStoreOp && mClearValue == inOther.mClearValue;
	}
};
