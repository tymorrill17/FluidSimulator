#pragma once
#include "vulkan/vulkan.h"
#include "device.h"
#include "NonCopyable.h"

class Semaphore {
public:
	Semaphore(Device& device, VkSemaphoreCreateFlags flags = 0U);
	~Semaphore();

	inline VkSemaphore handle() { return _semaphore; }

	void cleanup();

private:
	Device& _device;
	VkSemaphore _semaphore;
	VkSemaphoreCreateFlags _flags;
};

class Fence {
public:
	Fence(Device& device, VkFenceCreateFlags flags = 0U);
	~Fence();

	inline VkFence handle() { return _fence; }

	void cleanup();

private:
	Device& _device;
	VkFence _fence;
	VkFenceCreateFlags _flags;
};
