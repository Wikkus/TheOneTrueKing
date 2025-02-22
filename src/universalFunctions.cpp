#include "universalFunctions.h"
#include "gameEngine.h"
#include "objectBase.h"

Vector2<float> UniversalFunctions::GetCursorPosition() {
	SDL_GetMouseState(&_intPosition.x, &_intPosition.y);
	return Vector2<float>(_intPosition.x, _intPosition.y);
}

bool UniversalFunctions::IsInDistance(const Vector2<float>& positionA, const Vector2<float>& positionB, const float& distance) {
	return Vector2<float>::distanceBetweenVectors(positionA, positionB) <= distance;
}

bool UniversalFunctions::OutsideBorderX(const float& positionX, const float& offset) {
	return positionX - offset < 0.f || positionX + offset > windowWidth;
}

bool UniversalFunctions::OutsideBorderY(const float& positionY, const float& offset) {
	return positionY - offset < 0.f || positionY + offset > windowHeight;
}

float UniversalFunctions::Clamp(const float& a, const float& min, const float& max) {
	if (a < min) {
		return min;
	} else if (a > max) {
		return max;
	}
	return a;
}

float UniversalFunctions::WrapMax(const float& rotation, const float& maxValue) {
	return fmod(maxValue + fmod(rotation, maxValue), maxValue);
}
float UniversalFunctions::WrapMinMax(const float& rotation, const float& minValue, const float& maxValue) {
	return minValue + WrapMax(rotation - minValue, maxValue - minValue);
}

float UniversalFunctions::RandomBinomialFloat(const float& a, const float& b) {
	std::uniform_real_distribution<float> dist(a, b);
	return dist(randomEngine);
}

int UniversalFunctions::RandomBinomialInt(const int& a, const int& b) {
	std::uniform_int_distribution<int> dist(a, b);
	return dist(randomEngine);
}

int UniversalFunctions::ClosestPosition(const std::vector<Vector2<float>>& positions, const Vector2<float>& targetPosition) {
	_distance = FLT_MAX;
	_currentIndex = -1;

	for (unsigned int i = 0; i < positions.size(); i++) {
		_currentDistance = (positions[i] - targetPosition).absolute();
		if (_currentDistance < _distance) {
			_distance = _currentDistance;
			_currentIndex = i;
		}
	}
	return _currentIndex;
}

float UniversalFunctions::VectorAsOrientation(const Vector2<float>& direction) {
	_floatPosition = direction.normalized();
	return atan2f(_floatPosition.x, -_floatPosition.y);
}
Vector2<float> UniversalFunctions::OrientationAsVector(const float& orientation) {
	return Vector2<float>(-sinf(orientation), cosf(orientation)).normalized();
}

void UniversalFunctions::LimitVelocity(Vector2<float>& velocity, const float& maxAcceleration) {
	if (velocity.absolute() > maxAcceleration) {
		velocity = velocity.normalized() * maxAcceleration;
	}
}
