#pragma once
#include "vulkan/vulkan.h"
#include "renderer/sync.h"
#include "NonCopyable.h"
#include <functional>

class Device;
class Frame;

class Command : public NonCopyable {
public:
	Command(const Device& device, VkCommandPoolCreateFlags flags);
	~Command();

	Command(Command&& other) noexcept;
	Command& operator=(Command&& other) noexcept;

	inline VkCommandPool pool() const { return _commandPool; }
	inline VkCommandBuffer buffer() const { return _commandBuffer; }

	// @brief Allocates a command buffer from the command pool
	void allocateCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	// @brief Resets the command pool. Command buffers are not destroyed, just reset to an initial state
	void resetCommandPool();

	// @brief Begins the command buffer. Don't forget to end the command buffer too
	void begin();
	// @brief Ends the command buffer. This shouldn't be called unless the command buffer has been begun
	void end();
	// @brief Resets the command buffer. Not to be confused with resetting the command pool
	void reset(VkCommandBufferResetFlags flags = 0) const;

	// @brief Submits the current command buffer to the specified queue
	//
	// @param queue - Queue to submit the command buffer to
	// @param frame - The current frame waiting for rendering. This object contains the sync objects needed to submit properly
	void submitToQueue(VkQueue queue, Frame& frame);

	// @brief Populates a command buffer begin info struct
	static VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

protected:

	void cleanup(); 

	const Device& _device;

	VkCommandPool _commandPool;
	VkCommandBuffer _commandBuffer;
	VkCommandPoolCreateFlags _flags;
	bool _inProgress;
};

class ImmediateCommand : public Command {
public:
	ImmediateCommand(const Device& device, VkCommandPoolCreateFlags flags);

	inline Fence& fence() { return _submitFence; }

	// Submit the immediate command to the queue
	void submitToQueue(VkQueue queue);

	// @brief Immediately submit a command to the graphics queue
	void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	Fence _submitFence;
};