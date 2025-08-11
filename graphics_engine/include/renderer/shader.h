#pragma once
#include "vulkan/vulkan.h"
#include "device.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const Device& device, const std::string& filepath, VkShaderStageFlagBits stageFlag);
	~Shader();
	
	inline VkShaderModule module() const { return _shaderModule; }
	inline VkShaderStageFlagBits stage() const { return _shaderStageFlag; }

	// @brief Populates a pipeline shader stage create info struct
	//
	// @param flags - Bit flags to enable in the create info struct
	// @param shader - Shader module to be added to the pipeline
	static VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader);

private:
	const Device& _device;
	VkShaderModule _shaderModule;
	VkShaderStageFlagBits _shaderStageFlag;
};