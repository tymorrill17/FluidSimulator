#include "renderer/frame.h"
#include "renderer/command.h"

Frame::Frame(Device& device) :
    _device(device),
	_presentSemaphore(device),
	_renderSemaphore(device),
	_renderFence(device, VK_FENCE_CREATE_SIGNALED_BIT)
{}


