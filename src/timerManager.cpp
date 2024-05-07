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

void TimerManager::RemoveTimer(unsigned int timerIndex) {
	std::swap(_timers[timerIndex], _timers.back());
	_timers.pop_back();
}

std::shared_ptr<Timer> TimerManager::CreateTimer(float timeInSeconds) {
	std::shared_ptr<Timer> timer = std::make_shared<Timer>(timeInSeconds);
	_timers.emplace_back(timer);
	return timer;
}
