#pragma once
#include "vector2.h"

#include <vector>

class UniversalFunctions {
public:
	UniversalFunctions() {}
	~UniversalFunctions() {}
	
	Vector2<float> GetCursorPosition();

	bool IsInDistance(const Vector2<float>& positionA, const Vector2<float>& positionB, const float& distance);
	bool OutsideBorderX(const float& positionX, const float& offset);
	bool OutsideBorderY(const float& positionY, const float& offset);

	float Clamp(const float& a, const float& min, const float& max);
	float WrapMax(const float& rotation, const float& maxValue);
	float WrapMinMax(const float& rotation, const float& minValue, const float& maxValue);

	float RandomBinomialFloat(const float& a, const float& b);
	int RandomBinomialInt(const int& a, const int& b);

	int ClosestPosition(const std::vector<Vector2<float>>& positions, const Vector2<float>& targetPosition);
	
	float VectorAsOrientation(const Vector2<float>& direction);
	Vector2<float> OrientationAsVector(const float& orientation);

	void LimitVelocity(Vector2<float>& velocity, const float& maxAcceleration);

private:
	float _distance = 0.f;
	float _currentDistance = 0.f;
	int _currentIndex = -1;

	Vector2<float> _floatPosition;

	Vector2<int> _cursorPosition;
	Vector2<int> _intPosition;

};