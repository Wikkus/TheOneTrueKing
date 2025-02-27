#pragma once

class Timer {
public:
	Timer() {}
	~Timer() {}
	
	void Update();

	const bool GetIsActive() const;
	const bool GetIsFinished() const;

	void SetTimer(const bool& isActive, const bool& isFinished);
	void SetTimerActive(const bool& isActive);
	void SetTimerFinished(const bool& isFinished);
	
	void ResetTimer();
	void SetTimeInSeconds(const float& timeInSecond);


private:
	bool _isActive = false;
	bool _isFinished = false;

	float _timeInSeconds = 0.f;	
	float _currentTime = 0.f;

};