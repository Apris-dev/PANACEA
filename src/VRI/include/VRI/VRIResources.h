#pragma once

#include <forward_list>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include "VkBootstrap.h"
#include "VRI.h"

#include "basic/core/Common.h"
#include "sstl/Vector.h"

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

enum class EShaderStage : uint8 {
	VERTEX,
	FRAGMENT,
	COMPUTE
};

enum class EBlendMode : uint8 {
	NONE,
	ADDITIVE,
	ALPHA_BLEND
};

enum class EDepthTestMode : uint8 {
	NORMAL,
	BEHIND,
	FRONT
};

struct SPipelineCreateInfo {
	VkShaderModule vertexModule;
	VkShaderModule fragmentModule;
	VkPrimitiveTopology mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	VkPolygonMode mPolygonMode = VK_POLYGON_MODE_FILL;
	float mLineWidth = 1.f;
	VkCullModeFlags mCullMode = VK_CULL_MODE_FRONT_BIT;
	VkFrontFace mFrontFace = VK_FRONT_FACE_CLOCKWISE;
	bool mUseMultisampling = false;
	EBlendMode mBlendMode = EBlendMode::NONE;
	EDepthTestMode mDepthTestMode = EDepthTestMode::NORMAL;
	VkFormat mColorFormat;
	VkFormat mDepthFormat;
};

class CVertexAttributeArchive {

	struct VertexAttributeFormat {
		VkVertexInputRate inputRate;
		TVector<VkFormat> formats;
	};

public:

	void createBinding(const VkVertexInputRate InputRate) {
		m_Formats.push(VertexAttributeFormat{InputRate, TVector<VkFormat>{}});
	}

	VkPipelineVertexInputStateCreateInfo get() {
		m_Bindings.clear();
		m_Attributes.clear();

		uint32 location = 0;
		uint32 binding = 0;
		for (auto& [inputRate, formats] : m_Formats) {
			uint32 stride = 0;
			for (uint32 current = 0; current < formats.getSize(); ++current, ++location) {
				const auto& format = formats[current];
				m_Attributes.push(VkVertexInputAttributeDescription{
					location,
					binding,
					format,
					stride
				});
				//stride += getVkFormatSize(format);
			}
			m_Bindings.push({
				.binding = binding,
				.stride = stride,
				.inputRate = inputRate
			});
			binding++;
		}

		return
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = static_cast<uint32>(m_Bindings.getSize()),
			.pVertexBindingDescriptions = m_Bindings.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32>(m_Attributes.getSize()),
			.pVertexAttributeDescriptions = m_Attributes.data()
		};
	}

	friend CVertexAttributeArchive& operator<<(CVertexAttributeArchive& inArchive, VkFormat inFormat) {
		inArchive.m_Formats.top().formats.push(inFormat);
		return inArchive;
	}

private:

	// Stored here because the data needs to last the lifetime of this object
	TVector<VkVertexInputBindingDescription> m_Bindings;
	TVector<VkVertexInputAttributeDescription> m_Attributes;

	TVector<VertexAttributeFormat> m_Formats;
};

struct SVRIResource {

	EXPORT SVRIResource();
	virtual ~SVRIResource() = default;

	// No copying vulkan resources, since most need to be deleted, and if they are copied, could be deleted more than once
	SVRIResource(const SVRIResource&) = delete;
	SVRIResource& operator=(const SVRIResource&) = delete;

	virtual std::function<void()> getDestroyer() { return {}; }

	EXPORT void release();
};

struct CCommandPool : SVRIResource {

	enum Flags {
		NONE = 0,
		TRANSIENT = 0x00000001,
		RESET_COMMAND_BUFFER = 0x00000002,
		PROTECTED = 0x00000004
	};

	CCommandPool() = default;

	[[nodiscard]] VkCommandPool get() const { return mCommandPool; }

	virtual std::function<void()> getDestroyer() override {
		return [commandPool = mCommandPool] {
			vkDestroyCommandPool(CVRI::get()->getDevice()->device, commandPool, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CCommandPool> VRICreateCommandPool(uint32, Flags);
	VkCommandPool mCommandPool = nullptr;
};

EXPORT TUnique<CCommandPool> VRICreateCommandPool(uint32 queueFamilyIndex, CCommandPool::Flags flags = CCommandPool::NONE);

struct CDescriptorPool : SVRIResource {

	enum Flags {
		NONE = 0,
		FREE_DESCRIPTOR_SET = 0x00000001,
		UPDATE_AFTER_BIND = 0x00000002,
		HOST_ONLY = 0x00000004,
		ALLOW_OVERALLOCATION_SETS = 0x00000008,
		ALLOW_OVERALLOCATION_POOLS = 0x00000010
	};

	CDescriptorPool() = default;

	[[nodiscard]] VkDescriptorPool get() const { return mDescriptorPool; }

	virtual std::function<void()> getDestroyer() override {
		return [descriptorPool = mDescriptorPool] {
			vkDestroyDescriptorPool(CVRI::get()->getDevice()->device, descriptorPool, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32, uint32, const VkDescriptorPoolSize*, Flags);
	VkDescriptorPool mDescriptorPool = nullptr;
};

EXPORT TUnique<CDescriptorPool> VRICreateDescriptorPool(uint32 maxSets, uint32 poolSizeCount, const VkDescriptorPoolSize* poolSizes, CDescriptorPool::Flags flags = CDescriptorPool::NONE);

struct CDescriptorSetLayout : SVRIResource {

	enum Flags {
		NONE = 0,
		UPDATE_AFTER_BIND_POOL = 0x00000002,
		PUSH_DESCRIPTOR = 0x00000001,
		DESCRIPTOR_BUFFER = 0x00000010,
		EMBEDDED_IMMUTABLE_SAMPLERS = 0x00000020,
		INDIRECT_BINDABLE = 0x00000080,
		HOST_ONLY_POOL = 0x00000004,
		PER_STAGE = 0x00000040
	};

	CDescriptorSetLayout() = default;

	[[nodiscard]] VkDescriptorSetLayout& get() { return mDescriptorSetLayout; }

	virtual std::function<void()> getDestroyer() override {
		return [descriptorSetLayout = mDescriptorSetLayout] {
			vkDestroyDescriptorSetLayout(CVRI::get()->getDevice()->device, descriptorSetLayout, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32, const VkDescriptorSetLayoutBinding*, Flags);
	VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
};

EXPORT TUnique<CDescriptorSetLayout> VRICreateDescriptorSetLayout(uint32 bindingCount, const VkDescriptorSetLayoutBinding* bindings, CDescriptorSetLayout::Flags flags = CDescriptorSetLayout::NONE);

struct CFence : SVRIResource {

	enum Flags {
		NONE = 0,
		SIGNALED = 0x00000001
	};

	CFence() = default;

	[[nodiscard]] VkFence& get() { return mFence; }

	virtual std::function<void()> getDestroyer() override {
		return [fence = mFence] {
			vkDestroyFence(CVRI::get()->getDevice()->device, fence, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CFence> VRICreateFence(Flags);
	VkFence mFence = nullptr;
};

EXPORT TUnique<CFence> VRICreateFence(CFence::Flags flags = CFence::NONE);

struct CPipelineLayout : SVRIResource {

	enum Flags {
		NONE = 0,
		INDEPENDENT_SETS = 0x00000002
	};

	CPipelineLayout() = default;

	[[nodiscard]] VkPipelineLayout get() const { return mPipelineLayout; }

	virtual std::function<void()> getDestroyer() override {
		return [pipelineLayout = mPipelineLayout] {
			vkDestroyPipelineLayout(CVRI::get()->getDevice()->device, pipelineLayout, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CPipelineLayout> VRICreatePipelineLayout(uint32, const VkDescriptorSetLayout*, uint32, const VkPushConstantRange*, Flags);
	VkPipelineLayout mPipelineLayout = nullptr;
};

EXPORT TUnique<CPipelineLayout> VRICreatePipelineLayout(uint32 setLayoutCount, const VkDescriptorSetLayout* setLayouts, uint32 pushConstantRangeCount, const VkPushConstantRange* pushConstantRages, CPipelineLayout::Flags flags = CPipelineLayout::NONE);

//TODO: rename
struct SPipeline : SVRIResource {

	SPipeline() = default;

	[[nodiscard]] VkPipeline get() const { return mPipeline; }

	[[nodiscard]] VkPipelineLayout getLayout() const { return mLayout->get(); }

	EXPORT void bind(VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint) const;

	EXPORT virtual std::function<void()> getDestroyer() override;

private:
	EXPORT friend TUnique<SPipeline> VRICreatePipeline(const SPipelineCreateInfo&, CVertexAttributeArchive&, const TUnique<CPipelineLayout>&);
	VkPipeline mPipeline = nullptr;
	TFrail<CPipelineLayout> mLayout;
};

EXPORT TUnique<SPipeline> VRICreatePipeline(const SPipelineCreateInfo& inCreateInfo, CVertexAttributeArchive& inAttributes, const TUnique<CPipelineLayout>& inLayout);

struct CRenderPass : SVRIResource {

	enum Flags {
		NONE = 0,
		TRANSFORM = 0x00000002,
		PER_LAYER_FRAGMENT_DENSITY = 0x00000004
	};

	CRenderPass() = default;

	[[nodiscard]] VkRenderPass& get() { return mRenderPass; }

	virtual std::function<void()> getDestroyer() override {
		return [renderPass = mRenderPass] {
			vkDestroyRenderPass(CVRI::get()->getDevice()->device, renderPass, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CRenderPass> VRICreateRenderPass(
		uint32,
		const VkAttachmentDescription*,
		uint32,
		const VkSubpassDescription*,
		uint32,
		const VkSubpassDependency*,
		Flags
	);
	VkRenderPass mRenderPass = nullptr;
};

EXPORT TUnique<CRenderPass> VRICreateRenderPass(
	uint32 attachmentCount,
    const VkAttachmentDescription* attachments,
    uint32 subpassCount,
    const VkSubpassDescription* subpasses,
    uint32 dependencyCount,
    const VkSubpassDependency* dependencies,
	CRenderPass::Flags flags = CRenderPass::NONE
);

struct CSampler : SVRIResource {

	CSampler() = default;

	[[nodiscard]] VkSampler& get() { return mSampler; }

	virtual std::function<void()> getDestroyer() override {
		return [sampler = mSampler] {
			vkDestroySampler(CVRI::get()->getDevice()->device, sampler, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo&);
	VkSampler mSampler = nullptr;
};

EXPORT TUnique<CSampler> VRICreateSampler(const VkSamplerCreateInfo& inCreateInfo);

struct CSemaphore : SVRIResource {

	enum Flags {
		NONE = 0,
		SIGNALED = 0x00000001
	};

	CSemaphore() = default;

	[[nodiscard]] VkSemaphore& get() { return mSemaphore; }

	virtual std::function<void()> getDestroyer() override {
		return [semaphore = mSemaphore] {
			vkDestroySemaphore(CVRI::get()->getDevice()->device, semaphore, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CSemaphore> VRICreateSemaphore(Flags);
	VkSemaphore mSemaphore = nullptr;
};

EXPORT TUnique<CSemaphore> VRICreateSemaphore(CSemaphore::Flags flags = CSemaphore::NONE);

struct CShaderModule : SVRIResource {

	enum Flags {
		NONE = 0,
		DEVICE_ONLY = 0x00000001
	};

	CShaderModule() = default;

	[[nodiscard]] VkShaderModule& get() { return mShaderModule; }

	virtual std::function<void()> getDestroyer() override {
		return [shaderModule = mShaderModule] {
			vkDestroyShaderModule(CVRI::get()->getDevice()->device, shaderModule, nullptr);
		};
	}

private:
	EXPORT friend TUnique<CShaderModule> VRICreateShaderModule(size_t, const uint32*, Flags);
	VkShaderModule mShaderModule = nullptr;
};

EXPORT TUnique<CShaderModule> VRICreateShaderModule(size_t codeSize, const uint32* code, CShaderModule::Flags flags = CShaderModule::NONE);

struct CDescriptorSet : SVRIResource {

	CDescriptorSet() = default;

	[[nodiscard]] VkDescriptorSet& get() { return mDescriptorSet; }

	void bind(const VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint, const VkPipelineLayout inPipelineLayout, const uint32 inFirstSet, const uint32 inDescriptorSetCount) const {
		vkCmdBindDescriptorSets(cmd, inBindPoint,inPipelineLayout, inFirstSet, inDescriptorSetCount, &mDescriptorSet, 0, nullptr);
	}

private:
	EXPORT friend TUnique<CDescriptorSet> VRICreateDescriptorSet(VkDescriptorPool, uint32, const VkDescriptorSetLayout*);
	VkDescriptorSet mDescriptorSet = nullptr;
};

EXPORT TUnique<CDescriptorSet> VRICreateDescriptorSet(VkDescriptorPool descriptorPool, uint32 descriptorSetCount, const VkDescriptorSetLayout* setLayouts);

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

// Holds the resources needed for mesh rendering
struct SVRIMeshBuffer {

	SVRIMeshBuffer() = default;
	EXPORT SVRIMeshBuffer(size_t indicesSize, size_t verticesSize);

	TUnique<SVRIBuffer> indexBuffer = nullptr;
	TUnique<SVRIBuffer> vertexBuffer = nullptr;
};

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