#pragma once
#include "vector2.h"

#include <vector>

struct CostAndSlot;
struct CharacterAndSlots;

struct AnchorPoint {
	Vector2<float> position = { 0.f , 0.f };
	Vector2<float> targetPosition = { 0.f , 0.f };

	Vector2<float> direction = { 0.f , 0.f };

	float distance = 0.f;
	float movementSpeed = 60.f;
	float orientation = 0.f;
};

struct StaticCharacter {
	Vector2<float> position = { 0.f , 0.f };
	float orientation = 0.f;
};

bool IsInDistance(Vector2<float> positionA, Vector2<float> positionB, float distance);
bool OutOfBorderX(float positionX, float offset);
bool OutOfBorderY(float positionY, float offset);

Vector2<float> GetCursorPosition();
Vector2<float> OrientationAsVector(float orientation);
Vector2<float> RotateDirection(float degree, Vector2<float> direction);
Vector2<float> RotateVector(float degree, Vector2<float> startPoint, Vector2<float> endPoint);

Vector2<float> LimitVelocity(Vector2<float> velocity, float maxAcceleration);

Vector2<float> PointBetweenVectors(Vector2<float> fromVector, Vector2<float> toVector, float range);

float RandomBinomalFloat(float a, float b);
int RandomBinomalInt(int a, int b);

float VectorAsOrientation(Vector2<float> direction);

float WrapMax(float rotation, float maxValue);
float WrapMinMax(float rotation, float minValue, float maxValue);

int ClosestPosition(std::vector<Vector2<float>> positions, Vector2<float> targetPosition);

//int BinarySearch(std::vector<std::shared_ptr<ObjectBase>> objectVector, int low, int high, int targetID);

int Partition(std::vector<float>& objectVector,  int start, int end);
void QuickSort(std::vector<float>& objectVector, int start, int end);

int Partition(std::vector<CostAndSlot>& objectVector,  int start, int end);
void QuickSort(std::vector<CostAndSlot>& objectVector, int start, int end);

int Partition(std::vector<CharacterAndSlots>& objectVector,  int start, int end);
void QuickSort(std::vector<CharacterAndSlots>& objectVector, int start, int end);