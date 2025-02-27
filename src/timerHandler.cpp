#include "timerHandler.h"
#include "objectPool.h"

TimerHandler::TimerHandler() {
	_timerPool = std::make_shared<ObjectPool<std::shared_ptr<Timer>>>(_amountTimers);
}

void TimerHandler::Init() {
	for (unsigned int i = 0; i < _amountTimers; i++) {
		CreateTimer();
	}
}

void TimerHandler::Update() {
	for (unsigned int i = 0; i < _activeTimers.size(); i++) {
		_activeTimers[i]->Update();
	}
}

void TimerHandler::RemoveAllTimers() {
	while (_activeTimers.size() > 0) {
		_timerPool->PoolObject(_activeTimers.back());
		_activeTimers.pop_back();
	}
}

void TimerHandler::RemoveTimer(const unsigned int& timerIndex) {
	std::swap(_activeTimers[timerIndex], _activeTimers.back());
	_timerPool->PoolObject(_activeTimers.back());
	_activeTimers.pop_back();
}

std::shared_ptr<Timer> TimerHandler::SpawnTimer(const float& timeInSeconds, const bool& isActive, const bool& isFinished) {
	if (_timerPool->IsEmpty()) {
		CreateTimer();
	}
	_activeTimers.emplace_back(_timerPool->SpawnObject());
	_activeTimers.back()->SetTimeInSeconds(timeInSeconds);
	_activeTimers.back()->SetTimer(isActive, isFinished);
	return _activeTimers.back();
}

void TimerHandler::CreateTimer() {
	_timerPool->PoolObject(std::make_shared<Timer>());
}
