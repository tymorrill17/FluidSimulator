#include "renderer/sync.h"

Semaphore::Semaphore(const Device& device, VkSemaphoreCreateFlags flags) : _device(device), _flags(flags), _semaphore(VK_NULL_HANDLE) {
	VkSemaphoreCreateInfo semaphoreInfo{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = _flags
	};
	if (vkCreateSemaphore(_device.device(), &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create semaphore!");
	}
}

Semaphore::~Semaphore() {
	cleanup();
}

void Semaphore::cleanup() {
	vkDestroySemaphore(_device.device(), _semaphore, nullptr);
}

Semaphore::Semaphore(Semaphore&& other) noexcept : _device(other._device), _flags(other._flags), _semaphore(other._semaphore) {
	other._semaphore = VK_NULL_HANDLE;
	other._flags = 0;
}

Semaphore& Semaphore::operator=(Semaphore&& other) noexcept {
	if (this != &other) {
		cleanup();

		_flags = other._flags;
		_semaphore = other._semaphore;

		other._flags = 0;
		other._semaphore = VK_NULL_HANDLE;
	}
	return *this;
}

Fence::Fence(const Device& device, VkFenceCreateFlags flags) : _device(device), _flags(flags), _fence(VK_NULL_HANDLE) {
	VkFenceCreateInfo fenceInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = _flags,
	};
	if (vkCreateFence(_device.device(), &fenceInfo, nullptr, &_fence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create fence!");
	}
}

Fence::~Fence() {
	cleanup();
}

void Fence::cleanup() {
	vkDestroyFence(_device.device(), _fence, nullptr);
}

Fence::Fence(Fence&& other) noexcept : _device(other._device), _flags(other._flags), _fence(other._fence) {
	other._fence = VK_NULL_HANDLE;
	other._flags = 0;
}

Fence& Fence::operator=(Fence&& other) noexcept {
	if (this != &other) {
		cleanup();

		_flags = other._flags;
		_fence = other._fence;

		other._flags = 0;
		other._fence = VK_NULL_HANDLE;
	}
	return *this;
}