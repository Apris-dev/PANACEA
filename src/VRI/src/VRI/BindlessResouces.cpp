#include <array>

#include "VRI/BindlessResources.h"

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

//TODO: various uses of device singleton, need to remove
void CBindlessResources::init() {
	// Create Descriptor pool
	{
		auto poolSizes = {
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, gMaxTextures},
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, gMaxSamplers},
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, gMaxUniformBuffers},
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, gMaxStorageBuffers},
		};

		mDescriptorPool = VRICreateDescriptorPool(
			gMaxTextures + gMaxSamplers + gMaxUniformBuffers + gMaxStorageBuffers,
			(uint32)poolSizes.size(),
			poolSizes.begin(),
			CDescriptorPool::UPDATE_AFTER_BIND
		);
	}

	// Create Descriptor Set layout
	{
		constexpr VkDescriptorBindingFlags flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT; //VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT

		const auto inputFlags = {flags, flags, flags, flags};

		auto binding = {
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
			CDescriptorSetLayout::UPDATE_AFTER_BIND_POOL
		);
	}

	{
		constexpr uint32 maxBinding = gMaxStorageBuffers - 1;
		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
			.descriptorSetCount = 1,
			.pDescriptorCounts = &maxBinding
		};

		mDescriptorSet = VRICreateDescriptorSet(mDescriptorPool->get(), 1, &mDescriptorSetLayout->get());
	}

	// Create Basic Pipeline Layout
	// This includes the 8 Vector4f Push Constants (128 bytes) and the global DescriptorSetLayout
	{
		auto pushConstants = {
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
	}
}

void CBindlessResources::destroy() {
	mPipelineLayout.destroy();
	mDescriptorSetLayout.destroy();
	mDescriptorPool.destroy();
}
