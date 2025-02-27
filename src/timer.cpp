#include "timer.h"

#include "gameEngine.h"

const bool Timer::GetIsActive() const {
	return _isActive;
}

const bool Timer::GetIsFinished() const {
	return _isFinished;
}

void Timer::Update() {
	if (!_isFinished && _isActive) {
		if (_currentTime > 0) {
			_currentTime -= deltaTime;
		} else {
			_isActive = false;
			_isFinished = true;
		}
	}
}

void Timer::SetTimer(const bool& isActive, const bool& isFinished) {
	SetTimerActive(isActive);
	SetTimerFinished(isFinished);
}

void Timer::SetTimerActive(const bool& isActive) {
	_isActive = isActive;
}

void Timer::SetTimerFinished(const bool& isFinished) {
	_isFinished = isFinished;
}

void Timer::ResetTimer() {
	_currentTime = _timeInSeconds;
	_isActive = true;
	_isFinished = false;
}

void Timer::SetTimeInSeconds(const float& timeInSeconds) {
	_timeInSeconds = timeInSeconds;
	_currentTime = _timeInSeconds;
}
