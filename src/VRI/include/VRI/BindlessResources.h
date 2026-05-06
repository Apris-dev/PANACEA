#pragma once

#include "VRI/resources/VRIResources.h"

struct VkDescriptorPool_T;

struct CDescriptorSetLayout;
struct CDescriptorSet;
struct CPipelineLayout;
struct CDescriptorPool;

/*
 * Set Index Pool (SIP)
 * ----------------------------------------------------
 * Fixed slots, known statically, written at init
 *
 *  - Samplers. More do not need to be allocated once all combinations are met.
 *  - All SSBOs, like material data, vertex data, object data, meshlet data, lighting data, skinning data, or draw commands.
 *  - Some UBOs, Like Camera/projection data, time data.
 *  - Non-Runtime Textures, like the error material (index 0)
 */

struct SSetIndexPool {
	constexpr static uint32 gMaxSamplers = 256;
	constexpr static uint32 gMaxUniformBuffers = 4096;
	constexpr static uint32 gMaxStorageBuffers = 256;
	VkDescriptorPool mDescriptorPool = nullptr;
	VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
	VkDescriptorSet mDescriptorSet = nullptr;

	EXPORT void init();
	EXPORT void destroy() const;
};


/*
 * Fixed Size Pool (FSP)
 * ----------------------------------------------------
 * Has max sizes, slots are runtime determined, written on asset load/unload
 *
 *  - Runtime Textures
 *  - Texture Streaming mipmap Transitions
 */

struct SFixedSizePool {
	constexpr static uint32 gMaxTextures = 65536;
};

/*
 * Volatile Lifetime Pool (VLP)
 * ----------------------------------------------------
 * High frequency of create/destroy ops, unpredictable lifetimes or sizes
 *
 *  - Video/Movie Playback
 *  - LODs for skinned meshes where bone counts change between levels
 */

// TODO: migrate to three tiered model:
// SIP - Known Lifetimes
// FSP - Known Sizes
// VLP - Unknown lifetimes or sizes



// (Rough memory impact of 256 * 32B/64B = 8.192KB/16.384KB)

static uint32 gTextureBinding = 0;
static uint32 gSamplerBinding = 1;
static uint32 gUBOBinding = 2;
static uint32 gSSBOBinding = 3;

// A simple holder for bindless resources
class CBindlessResources final : public SObject {

	REGISTER_CLASS(CBindlessResources, SObject)
	//MAKE_LAZY_SINGLETON(CBindlessResources)

public:

	EXPORT static TUnique<CBindlessResources>& get();

	EXPORT virtual void init();

	EXPORT virtual void destroy();

	static TUnique<CPipelineLayout>& getBasicPipelineLayout() {
		return get()->mPipelineLayout;
	}

	static TUnique<CDescriptorSet>& getBindlessDescriptorSet() {
		return get()->mDescriptorSet;
	}

private:

	TUnique<CDescriptorPool> mDescriptorPool = nullptr;
	TUnique<CPipelineLayout> mPipelineLayout = nullptr;
	TUnique<CDescriptorSetLayout> mDescriptorSetLayout = nullptr;
	TUnique<CDescriptorSet> mDescriptorSet = nullptr;

};