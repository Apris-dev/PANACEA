#include "VRI/BindlessResources.h"

#include "vulkan/vulkan_core.h"
#include <array>
#include <VkBootstrap.h>

#include "sstl/Array.h"
#include "VRI/VRI.h"
#include "VRI/resources/DescriptorPool.h"
#include "VRI/resources/DescriptorSet.h"
#include "VRI/resources/DescriptorSetLayout.h"
#include "VRI/resources/PipelineLayout.h"
#include "VRI/resources/Sampler.h"

TUnique<CBindlessResources>& CBindlessResources::get() {
	static TUnique<CBindlessResources> bindlessResources{};
	return bindlessResources;
}

//TODO: permanent move for these
struct SPushConstants : TArray<Vector4f, 8> {
	SPushConstants() {
		resize([](size_t) { return Vector4f(0.f); });
	}
};

// TODO: Assumes flags (for now)
void SDescriptor::init(const TSpan<Pool>& pools, const DescriptorFlags inFlags) {
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
		for (const auto& pool : pools) {

			VkDescriptorType vkType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

			switch (pool.get().type) {
			case Pool::SAMPLER:
				vkType = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			case Pool::SAMPLED_IMAGE:
				vkType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;
			case Pool::UNIFORM_BUFFER:
				vkType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case Pool::STORAGE_BUFFER:
				vkType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			}

			poolSizes.push(VkDescriptorPoolSize{
				.type = vkType,
				.descriptorCount = pool.get().count
			});

			bindings.push(VkDescriptorSetLayoutBinding{
				.binding = pool.get().binding,
				.descriptorType = vkType,
				.descriptorCount = pool.get().count,
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

void SDescriptor::destroy() const {
	vkDestroyDescriptorSetLayout(CVRI::get()->getDevice()->device, mDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(CVRI::get()->getDevice()->device, mDescriptorPool, nullptr);
}



TUnique<CSampler> SSetIndexPool::createSampler(const std::string_view inName, const VkSamplerCreateInfo& inCreateInfo) {
	SSetIndexPool& setIndexPool = CBindlessResources::get()->setIndexPool;

	TUnique<CSampler> sampler = VRICreateSampler(inCreateInfo);

	const auto imageDescriptorInfo = VkDescriptorImageInfo{
		.sampler = sampler->get()
	};

	// Add samplers to tracking
	const uint32 currentSampler = setIndexPool.samplerIndexes.push(std::string(inName));

	const auto writeSet = VkWriteDescriptorSet{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = setIndexPool.descriptor.mDescriptorSet,
		.dstBinding = gSamplerBinding,
		.dstArrayElement = currentSampler,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
		.pImageInfo = &imageDescriptorInfo,
	};

	const auto sets = {writeSet};

	vkUpdateDescriptorSets(CVRI::get()->getDevice()->device, static_cast<uint32>(sets.size()), sets.begin(), 0, nullptr);

	return std::move(sampler);
}

//TODO: various uses of device singleton, need to remove
void CBindlessResources::init() {
	setIndexPool.init();
	fixedSizePool.init();

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
			{
				setIndexPool.descriptor.mDescriptorSetLayout,
				fixedSizePool.descriptor.mDescriptorSetLayout
			},
			{ pushConstants }
		);
	}
}

void CBindlessResources::destroy() {
	mPipelineLayout.destroy();
	fixedSizePool.destroy();
	setIndexPool.destroy();
}
