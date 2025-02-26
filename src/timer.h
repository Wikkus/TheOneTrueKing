#pragma once

class Timer {
public:
	Timer(const float& timeInSeconds);
	~Timer();

	const bool GetTimerActive() const;
	const bool GetTimerFinished() const;
	const bool IsWithinCertainTime(const float& decimalTime) const;
	
	const float GetCurrentTime() const;
	const float GetTimeInSeconds() const;

	void ActivateTimer();
	void DeactivateTimer();
	void ResetTimer();
	void Update();
	void SetTimeInSeconds(const float& timeInSecond);

private:
	bool _timerActive = false;
	bool _timerFinished = true;

	float _timeInSeconds = 0.f;	
	float _currentTime = 0.f;

};