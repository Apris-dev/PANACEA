#pragma once

#include <limits>

#include "VRI/resources/VRIResources.h"

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

/*
 * Fixed Size Pool (FSP)
 * ----------------------------------------------------
 * Has max sizes, slots are runtime determined, written on asset load/unload
 *
 *  - Runtime Textures
 *  - Texture Streaming mipmap Transitions
 */

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

// 65536 is a good number of textures to allow, as more would be overkill
// Fits within a 16 bit value
constexpr static uint32 gMaxTextures = 65536;
// (Rough memory impact of 65536 * 32B = ~2.1MB)
// This is high because we want a large # of textures

// There are very few types of samplers, so 256 is generous
// Since samplers are essentially static, they do not need to be in push constants
constexpr static uint32 gMaxSamplers = 256;
// (Rough memory impact of 256 * 32B/64B = 8.192KB/16.384KB)

// Uniform buffers tend to be fast to access but very small
// OpenGL spec states that uniform buffers guarantee up to 16 KB per buffer
// If per material data is wanted, 4096 is a good number to have
// Fits within an 12 bit value
constexpr static uint32 gMaxUniformBuffers = 4096;
// (Rough memory impact of 4096 * 16B/32B = 65.5KB/131.1KB)

// Shader Storage Buffers tend to be slower but larger
// OpenGL spec states that SSBOs guarantee up to 128 MB, but can be larger
// OpenGL spec also states only 8 SSBOs are guaranteed per shader stage (not true on modern hardware, good to know about however)
// Fits within a 8 bit balue
constexpr static uint32 gMaxStorageBuffers = 256;
// (Rough memory impact of 256 * 32B = 8.192KB)

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

	static TUnique<CDescriptorPool>& getBindlessDescriptorPool() {
		return get()->mDescriptorPool;
	}

	static TUnique<CPipelineLayout>& getBasicPipelineLayout() {
		return get()->mPipelineLayout;
	}

	static TUnique<CDescriptorSetLayout>& getBindlessDescriptorSetLayout() {
		return get()->mDescriptorSetLayout;
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