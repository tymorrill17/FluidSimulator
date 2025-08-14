#pragma once
#include "vulkan/vulkan.h"
#include "utility/logger.h"
#include "NonCopyable.h"
#include "device.h"

class Pipeline : public NonCopyable {
public:
	Pipeline();
	Pipeline(Device* device, VkPipeline pipeline, VkPipelineLayout pipelineLayout);
	~Pipeline();

    Pipeline(Pipeline&&) noexcept;
    Pipeline& operator=(Pipeline&&) noexcept;

	inline VkPipeline pipeline() { return _pipeline; }
	inline VkPipelineLayout pipelineLayout() { return _pipelineLayout; }

private:
    Device* _device;
    VkPipeline _pipeline; // The Vulkan render pipeline object
	VkPipelineLayout _pipelineLayout; // The pipeline layout used for interacting with the pipeline
};

namespace PipelineLayout {
	// @brief Create a default, blank VkPipelineLayoutCreateInfo struct
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& setLayouts = {}, const std::vector<VkPushConstantRange>& pushConstantRanges = {});

	// Creates a pipeline layout using the given create info
	VkPipelineLayout createPipelineLayout(Device& device, VkPipelineLayoutCreateInfo createInfo);
};
