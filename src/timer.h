#pragma once

class Timer {
public:
	Timer(float timeInSeconds);
	~Timer();

	const bool GetTimerActive() const;
	const bool GetTimerFinished() const;
	const bool IsWithinCertainTime(float decimalTime) const;
	
	const float GetCurrentTime() const;
	const float GetTimeInSeconds() const;

	void ActivateTimer();
	void DeactivateTimer();
	void ResetTimer();
	void Update();

private:
	bool _timerActive = true;
	bool _timerFinished = false;

	const float _timeInSeconds;	
	float _currentTime;

};