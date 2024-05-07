#pragma once
#include "timer.h"

#include <memory>
#include <vector>

class TimerManager {
public:
	TimerManager() {}
	~TimerManager();

	void Update();
	
	void RemoveTimer(unsigned int timerIndex);
	
	std::shared_ptr<Timer> CreateTimer(float timeInSeconds);

private:
	std::vector<std::shared_ptr<Timer>> _timers;
};