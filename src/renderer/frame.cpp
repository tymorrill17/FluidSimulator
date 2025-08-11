#include "renderer/frame.h"
#include "renderer/command.h"

Frame::Frame(const Device& device) : _device(device),
	_presentSemaphore(device),
	_renderSemaphore(device), 
	_renderFence(device, VK_FENCE_CREATE_SIGNALED_BIT),
	_command(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {}


Frame::Frame(Frame&& other) noexcept : _device(other._device),
	_presentSemaphore(std::move(other._presentSemaphore)),
	_renderSemaphore(std::move(other._renderSemaphore)),
	_renderFence(std::move(other._renderFence)),
	_command(std::move(other._command)) {}

Frame& Frame::operator=(Frame&& other) noexcept {
	if (this != &other) {
		_presentSemaphore = std::move(other._presentSemaphore);
		_renderSemaphore = std::move(other._renderSemaphore);
		_renderFence = std::move(other._renderFence);
		_command = std::move(other._command);
	}
	return *this;
}