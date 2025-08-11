#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
#include "utility/allocator.h"

Allocator* Allocator::_allocator;

Allocator::Allocator(Device& device, Instance& instance) : _device(device), _instance(instance) {
	VmaAllocatorCreateInfo allocatorCreateInfo{
		.physicalDevice = _device.physicalDevice(),
		.device = _device.device(),
		.instance = _instance.handle()
	};
	if (vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create the VMA allocator!");
	}
	_allocator = this;
}

Allocator::~Allocator() {
	vmaDestroyAllocator(_vmaAllocator);
}