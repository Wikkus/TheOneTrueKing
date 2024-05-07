#include "timer.h"

#include "gameEngine.h"

Timer::Timer(float timeInSeconds) : _timeInSeconds(timeInSeconds) {
	_currentTime = timeInSeconds;
}

Timer::~Timer() {}

const bool Timer::GetTimerActive() const {
	return _timerActive;
}

const bool Timer::GetTimerFinished() const {
	return _timerFinished;
}

const bool Timer::IsWithinCertainTime(float decimalTime) const {
	return _currentTime <= _timeInSeconds * decimalTime;
}

const float Timer::GetCurrentTime() const {
	return _currentTime;
}

const float Timer::GetTimeInSeconds() const {
	return _timeInSeconds;
}

void Timer::ActivateTimer() {
	_timerActive = true;
}

void Timer::DeactivateTimer() {
	_timerActive = false;
	_timerFinished = false;
}

void Timer::ResetTimer() {
	_currentTime = _timeInSeconds;
	_timerActive = true;
	_timerFinished = false;
}

void Timer::Update() {
	if (!_timerFinished && _timerActive) {
		if (_currentTime > 0) {
			_currentTime -= deltaTime;
		} else {
			_timerActive = false;
			_timerFinished = true;
		}
	}
}

