#pragma once
#include "utility/window.h"
#include "utility/gui.h"
#include "utility/logger.h"
#include "NonCopyable.h"
#include <vector>
#include <map>
#include <iostream>

enum class InputEvent {
	leftMouseDown, leftMouseUp,
	rightMouseDown, rightMouseUp,
	spacebarDown, spacebarUp,
	rightArrowDown, rightArrowUp
};

class InputManager : public NonCopyable {
public:
	InputManager(Window& window);

	// @brief Process SDL inputs and delegate action
	void processInputs();

	void addListener(InputEvent inputEvent, std::function<void()> callback);

	glm::vec2 mousePosition() const { return _mousePosition; }

private:
	Window& _window;
	std::unordered_map<InputEvent, std::vector<std::function<void()>>> _listeners;

	void dispatchEvent(InputEvent event);
	void updateMousePosition(SDL_Event* e);
	glm::vec2 _mousePosition{ 0.0f, 0.0f };
};

