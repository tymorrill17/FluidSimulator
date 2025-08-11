#include "renderer/instance.h"
#include <sstream>

std::vector<const char*> Instance::validationLayers = {
	"VK_LAYER_KHRONOS_validation" // Standard validation layer preset
};
std::vector<const char*> Instance::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME // Necessary extension to use swapchains
};

Instance::Instance(const char* appName, const char* engineName, bool enableValidationLayers) : 
	enableValidationLayers(enableValidationLayers),
	instance(VK_NULL_HANDLE) {

	static Logger& logger = Logger::getLogger();
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but are not supported!");
	}

	// Find version of Vulkan
	uint32_t instanceVersion;
	vkEnumerateInstanceVersion(&instanceVersion);
	logger.reportVersion(instanceVersion);

	// User-specified info about application
	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = appName,
		.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
		.pEngineName = engineName,
		.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
		.apiVersion = instanceVersion
	};

	// Populate the instance create info
	VkInstanceCreateInfo instanceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo
	};

	// Request instance extensions
	std::vector<const char*> extensions;
	getRequiredInstanceExtensions(extensions, enableValidationLayers);
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

	// Request validation layers if enabled
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		// Request validation layers
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Instance::validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = Instance::validationLayers.data();

		DebugMessenger::populateDebugMessengerCreateInfo(debugCreateInfo);
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}
	else {
		instanceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a Vulkan instance!");
	}

	logger.print("Created Vulkan instance.");
}

Instance::~Instance() {
	vkDestroyInstance(instance, nullptr);
}

bool Instance::checkValidationLayerSupport() {
	static Logger& logger = Logger::getLogger();
	std::stringstream line;

	// Query the instance for supported validation layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> supportedLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, supportedLayers.data());

	logger.printLayers("Layers Supported by Instance:", supportedLayers);

	// Loop through the requested validation layers and confirm they are all supported
	for (const char* layer : Instance::validationLayers) {
		bool foundLayer = false;
		for (const auto& layerProperties : supportedLayers) {
			if (strcmp(layer, layerProperties.layerName) == 0) {
				foundLayer = true;
				break;
			}
		}
		if (!foundLayer) {
			line << "Layer \"" << layer << "\" is not supported!";
			logger.print(line.str());
			return false;
		}
	}
	logger.print("All requested layers are supported!");
	return true;
}

void Instance::getRequiredInstanceExtensions(std::vector<const char*>& extensions, bool validationLayers) {
	static Logger& logger = Logger::getLogger();

	Window::getRequiredInstanceExtensions(extensions);

	if (validationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	logger.printExtensions("Required Instance Extensions:", extensions);
}