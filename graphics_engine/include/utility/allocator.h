#pragma once
#include "vma/vk_mem_alloc.h"
#include "utility/logger.h"
#include "renderer/device.h"
#include "renderer/instance.h"
#include "NonCopyable.h"

class Allocator : public NonCopyable {
public:
	Allocator(Device& device, Instance& instance);
	~Allocator();

	inline VmaAllocator handle() const { return _vmaAllocator; }

private:
	// @brief The actual VMA allocator instance
	VmaAllocator _vmaAllocator;

	Device& _device;
	Instance& _instance;
};
