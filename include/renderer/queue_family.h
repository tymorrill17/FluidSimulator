#pragma once
#include <vulkan/vulkan.h>
#include "utility/logger.h"
#include <optional>
#include <vector>

struct QueueFamilyIndices {
	// @brief Draw command support
	std::optional<uint32_t> graphicsFamily;
	// @brief Drawing to surface support
	std::optional<uint32_t> presentFamily;

	// @brief Are there indices for both graphics and present queue families?
	//
	// @return True if both families have index values. False otherwise
	inline bool isComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

	// @brief Properties of the chosen GPU's queue families
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
};

class QueueFamily {
public:
	QueueFamily() = delete;
	~QueueFamily() = delete;

	// @brief Find the indices of queue families with support for graphics and present commands.
	// Note: I believe the same index may be chosen for both graphics and present queue families
	//
	// @param physicalDevice - Physical device to query for queue families
	// @param surface - Surface object to queue present support for
	// @return The QueueFamilyIndices struct which contains indices for the graphics and present queue families. These may both be the same number
	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};