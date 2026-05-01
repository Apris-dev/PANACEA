#include "VRI/resources/VRIBuffer.h"

#include <VkBootstrap.h>

#include "VRI/BindlessResources.h"
#include "VRI/VRI.h"
#include "VRI/resources/DescriptorSet.h"

SVRIBuffer::SVRIBuffer(const size_t inBufferSize, const VmaMemoryUsage inMemoryUsage, const VkBufferUsageFlags inBufferUsage)
: size(inBufferSize) {

	// allocate buffer
	const VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.size = inBufferSize,
		.usage = inBufferUsage
	};

	const VmaAllocationCreateInfo vmaallocInfo = {
		.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = inMemoryUsage
	};

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(CVRI::get()->getVkAllocator(), &bufferCreateInfo, &vmaallocInfo, &buffer, &allocation, &info));
}

void SVRIBuffer::makeGlobal() {
	// Update descriptors with new buffer
	//TODO: need some way of guaranteeing Buffer addresses so they don't have to be passed in push constants
	static uint32 gCurrentBufferAddress = 0;
	mBindlessAddress = gCurrentBufferAddress;
	gCurrentBufferAddress++;

	updateGlobal();

}

void SVRIBuffer::updateGlobal() const {
	//TODO: need some way of guaranteeing Buffer addresses so they don't have to be passed in push constants
	const auto bufferDescriptorInfo = VkDescriptorBufferInfo{
		.buffer = buffer,
		.offset = info.offset,
		.range = info.size
	};

	const auto writeSet = VkWriteDescriptorSet{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = CBindlessResources::getBindlessDescriptorSet()->get(),
		.dstBinding = gUBOBinding, //TODO: for now UBO bindings
		.dstArrayElement = mBindlessAddress,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &bufferDescriptorInfo,
	};
	vkUpdateDescriptorSets(CVRI::get()->getDevice()->device, 1, &writeSet, 0, nullptr);
}

std::function<void()> SVRIBuffer::getDestroyer() {
	return [buffer = buffer, allocation = allocation] {
		msgs("Destroy Buffer");
		vmaDestroyBuffer(CVRI::get()->getVkAllocator(), buffer, allocation);
	};
}

void SVRIBuffer::mapData(void** data) const {
	vmaMapMemory(CVRI::get()->getVkAllocator(), allocation, data);
}

void SVRIBuffer::unMapData() const {
	vmaUnmapMemory(CVRI::get()->getVkAllocator(), allocation);
}