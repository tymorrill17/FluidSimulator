#pragma once
#include <vulkan/vulkan.h>
#include "renderer/queue_family.h"
#include <string>
#include <vector>
#include <iostream>
#include "NonCopyable.h"

// @brief Manages debugging messaging and console logging
class Logger : public NonCopyable {
public:
	// @brief Get the static logger instance, or create one if non-existent
	static Logger& getLogger();

	// @brief activates the logging
	void activate();

	// @brief deactivate logging
	void deactivate();

	inline bool isActive() const { return _active; }

	// @brief Print a message to the console
	//
	// @param message - Message to be printed
	void print(std::string message) const;

	// @brief Print a list of strings
	void printList(std::vector<const char*>& list) const;

	// @brief Print a list of validation layers
	void printLayers(const char* layerCategory, std::vector<VkLayerProperties>& layers) const;
	void printLayers(const char* layerCategory, std::vector<const char*>& layers) const;

	// @brief Print a list of extensions
	void printExtensions(const char* extensionCategory, std::vector<VkExtensionProperties>& extensions) const;
	void printExtensions(const char* extensionCategory, std::vector<const char*>& extensions) const;

	// @brief Print a list of physical devices
	void printDevices(std::vector<VkPhysicalDevice>& devices) const;

	// @brief Print the Vulkan version number
	void reportVersion(uint32_t version) const;

	// @brief Print details about the QueueFamilyIndices
	void log(struct QueueFamilyIndices& indices) const;
	// @brief Print details about the physical device
	void log(struct VkPhysicalDeviceProperties& physDevice) const;

private:
	Logger();

	bool _active;
};

