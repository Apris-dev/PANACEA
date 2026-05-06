#include "VRI/BindlessResources.h"

#include "vulkan/vulkan_core.h"
#include <array>
#include <VkBootstrap.h>

#include "VRI/VRI.h"
#include "VRI/resources/DescriptorPool.h"
#include "VRI/resources/DescriptorSet.h"
#include "VRI/resources/DescriptorSetLayout.h"
#include "VRI/resources/PipelineLayout.h"

TUnique<CBindlessResources>& CBindlessResources::get() {
	static TUnique<CBindlessResources> bindlessResources{};
	return bindlessResources;
}

//TODO: permanent move for these
struct SPushConstants : std::array<Vector4f, 8> {
	SPushConstants() : array() {
		fill(Vector4f(0.f));
	}
};

struct SDescriptor {

	//TODO: look into PUSH_DESCRIPTOR for an entirely different system which does not outlive the command buffer
	enum Flags {
		CAN_FREE_SETS = 0x00000002, // Needs free, Implies VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
		CAN_UPDATE_SETS = 0x00000008, // descriptors updated while set is bound, implies UPDATE_AFTER_BIND
		CAN_HAVE_EMPTY_SLOTS = 0x00000010, // not all slots need to be populated
		PUSH_INLINE = 0x00000020 // no pool needed, recorded into command buffer (PUSH_DESCRIPTOR)
	};
	typedef uint32 DescriptorFlags;

	struct Pool {
		enum { // Type Alias
			SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER,
			SAMPLED_IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			STORAGE_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
		} type;
		uint32 count;
	};

	VkDescriptorPool mDescriptorPool = nullptr;
	VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
	VkDescriptorSet mDescriptorSet = nullptr;

	// TODO: Assumes flags (for now)
	void init(const TVector<Pool>& pools, const DescriptorFlags inFlags) {
		VkDescriptorPoolCreateFlags poolCreateFlags = 0;
		VkDescriptorBindingFlags bindingFlags = 0;
		VkDescriptorSetLayoutCreateFlags setLayoutCreateFlags = 0;

		// Define Flags Early
		{
			if (inFlags & CAN_FREE_SETS) {
				poolCreateFlags |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			}
			if (inFlags & CAN_UPDATE_SETS) {
				poolCreateFlags    |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
				bindingFlags |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
				setLayoutCreateFlags  |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
			}
			if (inFlags & CAN_HAVE_EMPTY_SLOTS) {
				bindingFlags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
			}
			if (inFlags & PUSH_INLINE) {
				setLayoutCreateFlags |= VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
			}
		}

		TVector<VkDescriptorPoolSize> poolSizes;
		poolSizes.reserve(pools.getSize());

		TVector<VkDescriptorSetLayoutBinding> bindings;
		bindings.reserve(pools.getSize());

		// Populate pool sizes and bindings first
		{
			uint32 currentBinding = 0;
			for (const auto& [type, count] : pools) {
				poolSizes.push(VkDescriptorPoolSize{
					.type = static_cast<VkDescriptorType>(type),
					.descriptorCount = count
				});

				bindings.push(VkDescriptorSetLayoutBinding{
					.binding = currentBinding++,
					.descriptorType = static_cast<VkDescriptorType>(type),
					.descriptorCount = count,
					.stageFlags = VK_SHADER_STAGE_ALL
				});
			}
		}

		// Create Descriptor Pool
		{
			const VkDescriptorPoolCreateInfo createInfo {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = poolCreateFlags,
				.maxSets = 1,
				.poolSizeCount = static_cast<uint32>(poolSizes.getSize()),
				.pPoolSizes = poolSizes.data(),
			};

			vkCreateDescriptorPool(CVRI::get()->getDevice()->device, &createInfo, nullptr, &mDescriptorPool);
		}

		{
			// Add input flags as vector of pools size
			TVector<uint32> inputFlags;
			inputFlags.resize(pools.getSize(), [&bindingFlags](size_t) {
				return bindingFlags;
			});

			const auto flagInfo = VkDescriptorSetLayoutBindingFlagsCreateInfo {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
				.bindingCount = static_cast<uint32>(inputFlags.getSize()),
				.pBindingFlags = inputFlags.data(),
			};

			const VkDescriptorSetLayoutCreateInfo createInfo {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext = &flagInfo,
				.flags = setLayoutCreateFlags,
				.bindingCount = static_cast<uint32>(bindings.getSize()),
				.pBindings = bindings.data()
			};

			vkCreateDescriptorSetLayout(CVRI::get()->getDevice()->device, &createInfo, nullptr, &mDescriptorSetLayout);
		}

		{
			const TVector setLayouts { mDescriptorSetLayout };

			const VkDescriptorSetAllocateInfo createInfo {
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.pNext = nullptr,
				.descriptorPool = mDescriptorPool,
				.descriptorSetCount = static_cast<uint32>(setLayouts.getSize()),
				.pSetLayouts = setLayouts.data()
		   };

			vkAllocateDescriptorSets(CVRI::get()->getDevice()->device, &createInfo, &mDescriptorSet);
		}
	}
};

void SSetIndexPool::init() {

	{
		SDescriptor SIP;

		const TVector pools {
			SDescriptor::Pool{SDescriptor::Pool::SAMPLER, gMaxSamplers},
			SDescriptor::Pool{SDescriptor::Pool::UNIFORM_BUFFER, gMaxUniformBuffers},
			SDescriptor::Pool{SDescriptor::Pool::STORAGE_BUFFER, gMaxStorageBuffers}
		};

		SIP.init(pools,
			SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}

	{
		SDescriptor FSP;

		const TVector pools {
			SDescriptor::Pool{SDescriptor::Pool::SAMPLED_IMAGE, 65535} //2^16
		};

		FSP.init(pools,
			SDescriptor::CAN_UPDATE_SETS | SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}

	// Doesn't work like this but whatever
	{
		SDescriptor VLP;

		const TVector pools {
			SDescriptor::Pool{SDescriptor::Pool::SAMPLED_IMAGE, 65535} //2^16
		};

		VLP.init(pools,
			SDescriptor::CAN_FREE_SETS | SDescriptor::CAN_HAVE_EMPTY_SLOTS
		);
	}

	// Doesn't work like this but whatever
	{
		SDescriptor Push;

		const TVector pools {
			SDescriptor::Pool{SDescriptor::Pool::SAMPLED_IMAGE, 65535} //2^16
		};

		Push.init(pools,
			SDescriptor::PUSH_INLINE
		);
	}
}

void SSetIndexPool::destroy() const {
	vkDestroyDescriptorSetLayout(CVRI::get()->getDevice()->device, mDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(CVRI::get()->getDevice()->device, mDescriptorPool, nullptr);
}

//TODO: various uses of device singleton, need to remove
void CBindlessResources::init() {
	// Create Descriptor pool
	/*{
		const TVector poolSizes {
			CDescriptorPool::PoolSize{ CDescriptorPool::SAMPLED_IMAGE, gMaxTextures},
			CDescriptorPool::PoolSize{ CDescriptorPool::SAMPLER, gMaxSamplers},
			CDescriptorPool::PoolSize{ CDescriptorPool::UNIFORM_BUFFER, gMaxUniformBuffers},
			CDescriptorPool::PoolSize{ CDescriptorPool::STORAGE_BUFFER, gMaxStorageBuffers},
		};

		mDescriptorPool = VRICreateDescriptorPool(
			gMaxTextures + gMaxSamplers + gMaxUniformBuffers + gMaxStorageBuffers,
			poolSizes,
			CDescriptorPool::Flags::UPDATE_AFTER_BIND
		);
	}

	// Create Descriptor Set layout
	{
		constexpr VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT; //VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT

		const auto inputFlags = {flags, flags, flags, flags};

		const auto binding = {
			VkDescriptorSetLayoutBinding {
				.binding = gTextureBinding,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.descriptorCount = gMaxTextures,
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			VkDescriptorSetLayoutBinding {
				.binding = gSamplerBinding,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
				.descriptorCount = gMaxSamplers,
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			VkDescriptorSetLayoutBinding {
				.binding = gUBOBinding,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = gMaxUniformBuffers,
				.stageFlags = VK_SHADER_STAGE_ALL,
			},
			VkDescriptorSetLayoutBinding {
				.binding = gSSBOBinding,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = gMaxStorageBuffers,
				.stageFlags = VK_SHADER_STAGE_ALL,
			}
		};

		const auto flagInfo = VkDescriptorSetLayoutBindingFlagsCreateInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
			.bindingCount = (uint32)inputFlags.size(),
			.pBindingFlags = inputFlags.begin(),
		};

		mDescriptorSetLayout = VRICreateDescriptorSetLayout(
			binding.size(),
			binding.begin(),
			CDescriptorSetLayout::Flags::UPDATE_AFTER_BIND_POOL
		);
	}

	{
		//TODO: not sure what is used for?
		constexpr uint32 maxBinding = gMaxStorageBuffers - 1;
		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
			.descriptorSetCount = 1,
			.pDescriptorCounts = &maxBinding
		};

		mDescriptorSet = VRICreateDescriptorSet(mDescriptorPool, { mDescriptorSetLayout });
	}

	// Create Basic Pipeline Layout
	// This includes the 8 Vector4f Push Constants (128 bytes) and the global DescriptorSetLayout
	{
		const auto pushConstants = {
			VkPushConstantRange{
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				.offset = 0,
				.size = sizeof(SPushConstants)
			}
		};

		mPipelineLayout = VRICreatePipelineLayout(
			1,
			&mDescriptorSetLayout->get(),
			(uint32)pushConstants.size(),
			pushConstants.begin()
		);
	}*/
}

void CBindlessResources::destroy() {
	mPipelineLayout.destroy();
	mDescriptorSetLayout.destroy();
	mDescriptorPool.destroy();
}
