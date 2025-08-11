#pragma once
#include <chrono>
#include <cmath>
#include "NonCopyable.h"

class Timer : public NonCopyable {
public:
	// @brief To be called every frame. Updates the frametime and avg fps counter
	void update();

	// @brief Get the static instance of the timer
	static Timer& getTimer() {
		static Timer instance;
		return instance;
	}

	inline float frameTime() const { return _frameTime; }
	inline float framesPerSecond() const { return _fps; }

private:
	Timer();

	float _frameTime;
	float _fps;

	std::chrono::steady_clock::time_point _currentTime;
	std::chrono::steady_clock::time_point _newTime;
};