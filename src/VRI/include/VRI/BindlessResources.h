#pragma once

#include "sstl/Array.h"
#include "sstl/Span.h"
#include "sstl/Vector.h"
#include "basic/core/Object.h"
#include "basic/core/Class.h"
#include "VRI/resources/VRIResources.h"

struct VkDescriptorPool_T;
struct VkDescriptorSetLayout_T;
struct VkDescriptorSet_T;

struct CPipelineLayout;

struct SDescriptor {

	VkDescriptorPool mDescriptorPool = nullptr;
	VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
	VkDescriptorSet mDescriptorSet = nullptr;

	//TODO: look into PUSH_DESCRIPTOR for an entirely different system which does not outlive the command buffer
	enum Flags {
		CAN_FREE_SETS = 0x00000001, // Needs free, Implies VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
		CAN_UPDATE_SETS = 0x00000002, // descriptors updated while set is bound, implies UPDATE_AFTER_BIND
		CAN_HAVE_EMPTY_SLOTS = 0x0000004, // not all slots need to be populated
		PUSH_INLINE = 0x0000008 // no pool needed, recorded into command buffer (PUSH_DESCRIPTOR)
	};
	typedef uint32 DescriptorFlags;

	struct Pool {
		uint32 binding;
		enum {
			SAMPLER,
			SAMPLED_IMAGE,
			UNIFORM_BUFFER,
			STORAGE_BUFFER
		} type;
		uint32 count;
	};

	EXPORT void init(const TSpan<Pool>& pools, DescriptorFlags inFlags);
	EXPORT void destroy() const;
};

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

	constexpr static uint32 gSamplerBinding = 0;
	constexpr static uint32 gUBOBinding = 1;
	constexpr static uint32 gSSBOBinding = 2;

	constexpr static TArray pools {
		SDescriptor::Pool{gSamplerBinding, SDescriptor::Pool::SAMPLER, gMaxSamplers},
		SDescriptor::Pool{gUBOBinding, SDescriptor::Pool::UNIFORM_BUFFER, gMaxUniformBuffers},
		SDescriptor::Pool{gSSBOBinding, SDescriptor::Pool::STORAGE_BUFFER, gMaxStorageBuffers}
	};

	EXPORT static TUnique<struct CSampler> createSampler(std::string_view inName, const VkSamplerCreateInfo& inCreateInfo);

	TVector<std::string> samplerIndexes;
	SDescriptor descriptor;

	void init() {
		descriptor.init(pools,
			SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}
	void destroy() const {
		descriptor.destroy();
	}
};

/*
 * Fixed Size Pool (FSP)
 * ----------------------------------------------------
 * Has max sizes, slots are runtime determined, written on asset load/unload
 *
 *  - Runtime Textures
 *  - Texture Streaming mipmap Transitions
 */

// TODO: in class allocation of stuff
struct SFixedSizePool {
	constexpr static uint32 gMaxTextures = 65536;
	constexpr static uint32 gMaxUniformBuffers = 128;

	constexpr static uint32 gTextureBinding = 0;
	constexpr static uint32 gUBOBinding = 1;

	constexpr static TArray pools {
		SDescriptor::Pool{gTextureBinding, SDescriptor::Pool::SAMPLED_IMAGE, gMaxTextures},
		SDescriptor::Pool{gUBOBinding, SDescriptor::Pool::UNIFORM_BUFFER, gMaxUniformBuffers}
	};

	SDescriptor descriptor;

	void init() {
		descriptor.init(pools,
		SDescriptor::CAN_UPDATE_SETS | SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}
	void destroy() const {
		descriptor.destroy();
	}
};

/*
 * Volatile Lifetime Pool (VLP)
 * ----------------------------------------------------
 * High frequency of create/destroy ops, unpredictable lifetimes or sizes
 *
 *  - Video/Movie Playback
 *  - LODs for skinned meshes where bone counts change between levels
 */

// TODO: make actually usable as volatile descriptor pool
struct SVolatileLifetimePool {
	constexpr static uint32 gMaxTextures = 65536;

	constexpr static uint32 gTextureBinding = 0;

	constexpr static TArray pools {
		SDescriptor::Pool{gTextureBinding, SDescriptor::Pool::SAMPLED_IMAGE, gMaxTextures}
	};

	SDescriptor descriptor;

	void init() {
		descriptor.init(pools,
		SDescriptor::CAN_FREE_SETS | SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}
	void destroy() const {
		descriptor.destroy();
	}
};

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

	static VkDescriptorSet getBindlessDescriptorSet() {
		return get()->fixedSizePool.descriptor.mDescriptorSet;
	}

private:

	SSetIndexPool setIndexPool;
	SFixedSizePool fixedSizePool;

	TUnique<CPipelineLayout> mPipelineLayout = nullptr;

};