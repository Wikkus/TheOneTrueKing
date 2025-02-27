#pragma once
#include "timer.h"

#include <memory>
#include <vector>

template<typename T> class ObjectPool;

class TimerHandler {
public:
	TimerHandler();
	~TimerHandler() {}

	void Init();
	void Update();
	
	void RemoveAllTimers();
	void RemoveTimer(const unsigned int& timerIndex);
	

	std::shared_ptr<Timer> SpawnTimer(const float& timeInSeconds, const bool& isActive, const bool& isFinished);
	void CreateTimer();

private:
	unsigned int _amountTimers = 3000;
	
	std::shared_ptr<ObjectPool<std::shared_ptr<Timer>>> _timerPool;
	std::vector<std::shared_ptr<Timer>> _activeTimers;
};