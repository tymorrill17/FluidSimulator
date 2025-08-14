#include "renderer/pipeline.h"
#include <iostream>
#include <utility>

Pipeline::Pipeline() :
	_device(nullptr), _pipeline(VK_NULL_HANDLE), _pipelineLayout(VK_NULL_HANDLE) {}

Pipeline::Pipeline(Device* device, VkPipeline pipeline, VkPipelineLayout pipelineLayout) :
	_device(device),
	_pipeline(pipeline),
	_pipelineLayout(pipelineLayout) {}

Pipeline::~Pipeline() {
	if (!_device) return;
	if (_pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(_device->handle(), _pipelineLayout, nullptr);
		_pipelineLayout = VK_NULL_HANDLE;
	}
	if (_pipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(_device->handle(), _pipeline, nullptr);
		_pipeline = VK_NULL_HANDLE;
	}
}

Pipeline::Pipeline(Pipeline&& other) noexcept :
    _device(std::move(other._device)),
    _pipeline(std::move(other._pipeline)),
    _pipelineLayout(std::move(other._pipelineLayout)) {

    other._device = nullptr;
    other._pipeline = VK_NULL_HANDLE;
    other._pipelineLayout = VK_NULL_HANDLE;
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept {
    if (this != &other) {
        _device = std::move(other._device);
        _pipeline = std::move(other._pipeline);
        _pipelineLayout = std::move(other._pipelineLayout);
        other._device = nullptr;
        other._pipeline = VK_NULL_HANDLE;
        other._pipelineLayout = VK_NULL_HANDLE;
    }
    return *this;
}

VkPipelineLayoutCreateInfo PipelineLayout::pipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges) {
    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
            .pSetLayouts = setLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
            .pPushConstantRanges = pushConstantRanges.data()
    };
    return createInfo;
}

VkPipelineLayout PipelineLayout::createPipelineLayout(Device& device, VkPipelineLayoutCreateInfo createInfo) {
	VkPipelineLayout pipelineLayout;
	if (vkCreatePipelineLayout(device.handle(), &createInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        Logger::logError("Failed to create pipeline layout!");
	}
	return pipelineLayout;
}

