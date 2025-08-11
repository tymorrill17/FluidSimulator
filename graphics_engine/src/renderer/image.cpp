#include "renderer/image.h"

Image::Image(VkImage image, VkImageView imageView, VkExtent3D extent, VkFormat format, VkImageLayout imageLayout) :
	_image(image), _imageView(imageView), _extent(extent), _format(format), _imageLayout(imageLayout) {}

void AllocatedImage::createAllocatedImage() {
	VkImageCreateInfo imageInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.imageType = VK_IMAGE_TYPE_2D, // Need to change this if I need 3D images
		.format = _format,
		.extent = _extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT, // Only applicable for target images
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = _usageFlags
	};

	VmaAllocationCreateInfo allocInfo{
		.usage = _memoryUsage,
		.requiredFlags = static_cast<VkMemoryPropertyFlags>(_vkMemoryUsage)
	};

	if (vmaCreateImage(_allocator.handle(), &imageInfo, &allocInfo, &_image, &_allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create and allocate image!");
	}

	VkImageSubresourceRange subresourceRange{
		.aspectMask = _aspectFlags,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageViewCreateInfo imageViewInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.image = _image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = _format,
		.subresourceRange = subresourceRange
	};

	if (vkCreateImageView(_device.device(), &imageViewInfo, nullptr, &_imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create allocated image view!");
	}
}

AllocatedImage::AllocatedImage(const Device& device, const Allocator& allocator) :
	Image(VK_NULL_HANDLE, VK_NULL_HANDLE, {0, 0, 0}, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED),
	_device(device), _allocator(allocator), _allocation(nullptr), _usageFlags(0), _memoryUsage(VMA_MEMORY_USAGE_UNKNOWN),
	_vkMemoryUsage(VMA_MEMORY_USAGE_UNKNOWN), _aspectFlags(VK_IMAGE_ASPECT_NONE) {}

AllocatedImage::AllocatedImage(const Device& device, const Allocator& allocator,
	VkExtent3D extent, VkFormat format, VkImageUsageFlags usageFlags,
	VmaMemoryUsage memoryUsage, VkMemoryAllocateFlags vkMemoryUsage,
	VkImageAspectFlags aspectFlags) :
	Image(VK_NULL_HANDLE, VK_NULL_HANDLE, extent, format, VK_IMAGE_LAYOUT_UNDEFINED),
	_device(device), _allocator(allocator), _allocation(nullptr), _usageFlags(usageFlags),
	_memoryUsage(memoryUsage), _vkMemoryUsage(vkMemoryUsage), _aspectFlags(aspectFlags) {

	createAllocatedImage();
}

void AllocatedImage::cleanup() {
	vkDestroyImageView(_device.device(), _imageView, nullptr);
	vmaDestroyImage(_allocator.handle(), _image, _allocation);
}

AllocatedImage::AllocatedImage(AllocatedImage&& other) noexcept : 
	Image(other._image, other._imageView, other._extent, other._format, other._imageLayout),
	_device(other._device), _allocator(other._allocator), _allocation(other._allocation),
	_usageFlags(other._usageFlags), _memoryUsage(other._memoryUsage), _vkMemoryUsage(other._vkMemoryUsage),
	_aspectFlags(other._aspectFlags) {
	other._allocation = nullptr;
	other._image = VK_NULL_HANDLE;
	other._imageView = VK_NULL_HANDLE;
	other._imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	other._extent = { 0, 0, 0 };
	other._format = VK_FORMAT_UNDEFINED;
}

AllocatedImage& AllocatedImage::operator=(AllocatedImage&& other) noexcept {
	if (this != &other) {
		// Clean up current object
		cleanup();

		_image = other._image;
		_imageView = other._imageView;
		_imageLayout = other._imageLayout;
		_extent = other._extent;
		_format = other._format;
		_allocation = other._allocation;
		_usageFlags = other._usageFlags;
		_memoryUsage = other._memoryUsage;
		_vkMemoryUsage = other._vkMemoryUsage;
		_aspectFlags = other._aspectFlags;

		other._image = VK_NULL_HANDLE;
		other._imageView = VK_NULL_HANDLE;
		other._imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		other._extent = { 0, 0, 0 };
		other._format = VK_FORMAT_UNDEFINED;
		other._allocation = nullptr;
	}
	return *this;
}

void AllocatedImage::recreate(VkExtent3D extent) {
	cleanup();
	_extent = extent;
	_imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_imageView = VK_NULL_HANDLE;
	createAllocatedImage();
}

SwapchainImage::SwapchainImage(const Device& device) :
	Image(VK_NULL_HANDLE, VK_NULL_HANDLE, {0, 0, 0}, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_UNDEFINED), _device(device) {}

SwapchainImage::SwapchainImage(const Device& device, VkImage image, VkExtent3D extent,
	VkFormat format) :
	Image(image, VK_NULL_HANDLE, extent, format, VK_IMAGE_LAYOUT_UNDEFINED), _device(device) {

	// Create associated image view. This is going to be a color aspect image view
	VkImageSubresourceRange subresourceRange{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageViewCreateInfo imageViewInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.image = _image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = _format,
		.subresourceRange = subresourceRange
	};

	if (vkCreateImageView(_device.device(), &imageViewInfo, nullptr, &_imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain image view!");
	}
}

void SwapchainImage::cleanup() {
	vkDestroyImageView(_device.device(), _imageView, nullptr);
}

SwapchainImage::SwapchainImage(SwapchainImage&& other) noexcept :
	Image(other._image, other._imageView, other._extent, other._format, other._imageLayout), _device(other._device) {
	other._image = VK_NULL_HANDLE;
	other._imageView = VK_NULL_HANDLE;
	other._imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	other._extent = { 0, 0, 0 };
	other._format = VK_FORMAT_UNDEFINED;
}

SwapchainImage& SwapchainImage::operator=(SwapchainImage&& other) noexcept {
	if (this != &other) {
		// Clean up current object
		cleanup();

		_image = other._image;
		_imageView = other._imageView;
		_imageLayout = other._imageLayout;
		_extent = other._extent;
		_format = other._format;

		other._image = VK_NULL_HANDLE;
		other._imageView = VK_NULL_HANDLE;
		other._imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		other._extent = { 0, 0, 0 };
		other._format = VK_FORMAT_UNDEFINED;
	}
	return *this;
}

void Image::transitionImage(Command& cmd, VkImageLayout newLayout) {
	VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageSubresourceRange subresourceRange{
		.aspectMask = aspectMask,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	VkImageMemoryBarrier2 imageBarrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.pNext = nullptr,
		.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		.oldLayout = _imageLayout,
		.newLayout = newLayout,
		.image = _image,
		.subresourceRange = subresourceRange
	};

	VkDependencyInfo dependencyInfo{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = nullptr,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &imageBarrier
	};

	vkCmdPipelineBarrier2(cmd.buffer(), &dependencyInfo);

	_imageLayout = newLayout;
}

void Image::copyImageOnGPU(Command& cmd, Image& src, Image& dst) {
	VkImageBlit2 blitRegion{
		.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
		.pNext = nullptr
	};

	blitRegion.srcOffsets[1].x = src.extent().width;
	blitRegion.srcOffsets[1].y = src.extent().height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dst.extent().width;
	blitRegion.dstOffsets[1].y = dst.extent().height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{
		.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
		.pNext = nullptr,
		.srcImage = src.image(),
		.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		.dstImage = dst.image(),
		.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.regionCount = 1,
		.pRegions = &blitRegion,
		.filter = VK_FILTER_LINEAR
	};

	vkCmdBlitImage2(cmd.buffer(), &blitInfo);
}

VkRenderingAttachmentInfoKHR Image::attachmentInfo(VkImageView imageView, VkClearValue* pClear, VkImageLayout imageLayout) {
	VkRenderingAttachmentInfoKHR renderingAttachmentInfo{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
		.pNext = nullptr,
		.imageView = imageView,
		.imageLayout = imageLayout,
		.loadOp = pClear != VK_NULL_HANDLE ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE
	};
	if (pClear) renderingAttachmentInfo.clearValue = *pClear;
	return renderingAttachmentInfo;
}