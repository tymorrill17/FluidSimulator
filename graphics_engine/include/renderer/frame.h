#pragma once
#include "vulkan/vulkan.h"
#include "NonCopyable.h"
#include "device.h"
#include "command.h"
#include "sync.h"

class Command;

class Frame {
public:
	Frame(Device& device);

	inline Semaphore& presentSemaphore() { return _presentSemaphore; }
	inline Semaphore& renderSemaphore() { return _renderSemaphore; }
	inline Fence& renderFence() { return _renderFence; }

private:
	Device& _device;
	Semaphore _presentSemaphore;
	Semaphore _renderSemaphore;
	Fence _renderFence;
};
