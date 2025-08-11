#include "utility/window.h"

Window::Window(glm::ivec2 dimensions, const std::string& name) :
	_windowExtent{ static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y) },
	_name(name), _instance(VK_NULL_HANDLE),
	_surface(VK_NULL_HANDLE),
	_windowShouldClose(false),
	_pauseRendering(false),
	_isFullscreen(false) {

	static Logger& logger = Logger::getLogger();
	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	// Create a window compatible with Vulkan surfaces
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	struct SDL_Window* window = SDL_CreateWindow(
		name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		windowFlags
	);

	if (window) {
		std::stringstream line;
		line << "Created a window titled \"" << name << "\" of size " << _windowExtent.width << "x" << _windowExtent.height << ".";
		logger.print(line.str());
	}
	else {
		throw std::runtime_error("Window creation failure!");
	}

	_window = window;
}

Window::~Window() {
	if (_surface) {
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	}
	SDL_DestroyWindow(_window);
}

Window::Window(Window&& other) noexcept : 
	_window(other._window), _windowExtent(other._windowExtent),
	_name(std::move(other._name)), _surface(other._surface),
	_instance(other._instance), _windowShouldClose(other._windowShouldClose),
	_pauseRendering(other._pauseRendering), _isFullscreen(other._isFullscreen) {
	other._window = nullptr;
	other._windowExtent = { 0, 0 };
	other._name.clear();
	other._surface = VK_NULL_HANDLE;
	other._instance = VK_NULL_HANDLE;
}

void Window::getRequiredInstanceExtensions(std::vector<const char*>& extensions) {
	uint32_t sdlRequiredExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(nullptr, &sdlRequiredExtensionCount, nullptr);
	std::vector<const char*> sdlRequiredExtensions(sdlRequiredExtensionCount);
	SDL_Vulkan_GetInstanceExtensions(nullptr, &sdlRequiredExtensionCount, sdlRequiredExtensions.data());

	extensions.assign(sdlRequiredExtensions.data(), sdlRequiredExtensions.data() + sdlRequiredExtensionCount);
}

void Window::updateSize() {
	int width, height;
	SDL_GetWindowSize(_window, &width, &height);
	_windowExtent.width = width;
	_windowExtent.height = height;
}

void Window::createSurface(VkInstance instance) {
	static Logger& logger = Logger::getLogger();

	_instance = instance;

	if (!SDL_Vulkan_CreateSurface(_window, instance, &_surface)) {
		throw std::runtime_error("Failed to create window surface!");
	}

	logger.print("Created SDL window surface for Vulkan");
}
