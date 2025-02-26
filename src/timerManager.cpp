#include "timerManager.h"

TimerManager::~TimerManager() {
	for (unsigned int i = 0; i < _timers.size(); i++) {
		RemoveTimer(i);
	}
}

void TimerManager::Update() {
	for (unsigned int i = 0; i < _timers.size(); i++) {
		_timers[i]->Update();
	}
}

void TimerManager::RemoveTimer(const unsigned int& timerIndex) {
	std::swap(_timers[timerIndex], _timers.back());
	_timers.pop_back();
}

std::shared_ptr<Timer> TimerManager::CreateTimer(const float& timeInSeconds) {
	_timers.emplace_back(std::make_shared<Timer>(timeInSeconds));
	return _timers.back();
}
