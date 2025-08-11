#include "renderer/swapchain.h"

void Swapchain::createSwapchain() {
	static Logger& logger = Logger::getLogger();

	// Query swapchain support details and set the extent to the same as the window
	_supportDetails = querySwapchainSupport(_device.physicalDevice(), _window.surface());
	_extent = setSwapchainExtent(_supportDetails.capabilities, _window);

	// Select the format and present modes of the swapchain
	VkSurfaceFormatKHR surfaceFormat = selectSwapchainSurfaceFormat(_supportDetails.formats);
	VkPresentModeKHR presentMode = selectSwapchainPresentMode(_supportDetails.presentModes);

	_imageFormat = surfaceFormat.format;

	// Set how many images will be in the swapchain. Typically this will be 2.
	_framesInFlight = _supportDetails.capabilities.minImageCount + 1;
	if (_supportDetails.capabilities.maxImageCount > 0 && _framesInFlight > _supportDetails.capabilities.maxImageCount)
		_framesInFlight = _supportDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR swapchainCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = _window.surface(),
		.minImageCount = _framesInFlight,
		.imageFormat = _imageFormat,
		.imageColorSpace = surfaceFormat.colorSpace,
		.imageExtent = _extent,
		.imageArrayLayers = 1, // number of layers in each image. This will usually be one unless doing something like stereoscopic 3D
		.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // We will be rendering to a draw image then transferring to the swapchain image
		.preTransform = _supportDetails.capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = presentMode,
		.clipped = VK_TRUE,
	};

	// Determine how to handle images across queue families
	const QueueFamilyIndices indices = _device.queueFamilyIndices();
	uint32_t queueFamilyIndies[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// Are there >1 queue families?
	if (indices.graphicsFamily != indices.presentFamily) {
		// If so, use concurrent mode. It is faster to transfer since no queue families own the image
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndies;
	}
	else {
		// Otherwise, use exclusive mode, as it is faster
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	if (vkCreateSwapchainKHR(_device.device(), &swapchainCreateInfo, nullptr, &_swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain!");
	}

	logger.print("Swapchain successfully created!");

	std::vector<VkImage> images;
	// Get the new swapchain's images
	vkGetSwapchainImagesKHR(_device.device(), _swapchain, &_framesInFlight, nullptr);
	images.resize(_framesInFlight);
	vkGetSwapchainImagesKHR(_device.device(), _swapchain, &_framesInFlight, images.data());

	// Now fill the _images vector, which creates the image views
	_images.reserve(_framesInFlight);
	VkExtent3D swapchainImageExtent{ _extent.width, _extent.height, 1 };
	for (VkImage image : images) {
		_images.emplace_back(_device, image, swapchainImageExtent, _imageFormat);
	}
}

Swapchain::Swapchain(const Device& device, Window& window) 
	: _swapchain(VK_NULL_HANDLE),
	_extent{0, 0}, _device(device),
	_window(window), _imageFormat(),
	_imageIndex(0), _resizeRequested(false) {
	createSwapchain();
}

void Swapchain::cleanup() {
	vkDestroySwapchainKHR(_device.device(), _swapchain, nullptr);
	_images.clear();
}

Swapchain::~Swapchain() {
	cleanup();
}

void Swapchain::recreate() {
	vkDeviceWaitIdle(_device.device()); // Wait for device to finish its tasks
	cleanup(); // Destroy old swapchain
	_window.updateSize(); // Update the window's extent (Maybe should move this elsewhere
	createSwapchain(); // Recreate the swapchain
	_resizeRequested = false;
}

VkSurfaceFormatKHR Swapchain::selectSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	// Try to find support for 8-bit SRGB color format
	for (const auto& format : availableFormats) {
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	// Otherwise, default to the first available
	return availableFormats[0];
}

VkPresentModeKHR Swapchain::selectSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	// Try to find MAILBOX present mode
	for (const auto& mode : availablePresentModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	// Otherwise, default to FIFO mode
	return VK_PRESENT_MODE_FIFO_KHR;
}

SwapchainSupportDetails Swapchain::querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
	SwapchainSupportDetails details;

	// Get the capabilities of both the device and the surface
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// Get the supported formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount > 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// Get the supported present modes
	uint32_t presentCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);
	if (presentCount > 0) {
		details.presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, details.presentModes.data());
	}

	return details;
}

VkExtent2D Swapchain::setSwapchainExtent(VkSurfaceCapabilitiesKHR capabilities, const Window& window) {

	// In Vulkan, an extent of size UINT32_MAX means the window resolution should be used
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else { // Otherwise, the window manager allows a custom resolution
		VkExtent2D actualExtent = window.extent();

		// Truncates the extent to within the surface capabilities
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		
		return actualExtent;
	}
}

void Swapchain::acquireNextImage(Semaphore* semaphore, Fence* fence) {
	VkResult e = vkAcquireNextImageKHR(_device.device(), _swapchain, 1000000000, semaphore->handle(), nullptr, &_imageIndex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
		_resizeRequested = true;
	} else if(e != VK_SUCCESS) {
		throw std::runtime_error("Failed to acquire next swapchain image!");
	}
}

void Swapchain::presentToScreen(VkQueue queue, Frame& frame, uint32_t imageIndex) {
	VkSemaphore waitSemaphore = frame.renderSemaphore().handle();
	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &waitSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &_swapchain,
		.pImageIndices = &imageIndex
	};
	VkResult e = vkQueuePresentKHR(queue, &presentInfo);
	if (e == VK_ERROR_OUT_OF_DATE_KHR || e == VK_SUBOPTIMAL_KHR) {
		_resizeRequested = true;
	} else if (e != VK_SUCCESS) {
		throw std::runtime_error("Failed to present to screen!");
	}
}