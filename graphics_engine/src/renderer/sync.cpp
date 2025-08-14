#include "renderer/sync.h"

Semaphore::Semaphore(Device& device, VkSemaphoreCreateFlags flags) : _device(device), _flags(flags), _semaphore(VK_NULL_HANDLE) {
	VkSemaphoreCreateInfo semaphoreInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = _flags
	};
	if (vkCreateSemaphore(_device.handle(), &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS) {
        Logger::logError("Failed to create semaphore!");
	}
}

Semaphore::~Semaphore() {
	vkDestroySemaphore(_device.handle(), _semaphore, nullptr);
}

Fence::Fence(Device& device, VkFenceCreateFlags flags) : _device(device), _flags(flags), _fence(VK_NULL_HANDLE) {
	VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = _flags,
	};
	if (vkCreateFence(_device.handle(), &fenceInfo, nullptr, &_fence) != VK_SUCCESS) {
        Logger::logError("Failed to create fence!");
	}
}

Fence::~Fence() {
	vkDestroyFence(_device.handle(), _fence, nullptr);
}

