#pragma once
#include "vector2.h"

#include <vector>

struct CostAndSlot;
struct CharacterAndSlots;

enum class BorderSide {
	Top,
	Left,
	Right,
	Bottom,
	Count
};

struct AnchorPoint {
	BorderSide borderSide = BorderSide::Count;
	Vector2<float> position = Vector2<float>(0.f, 0.f);
	float orientation = 0.f;
};

bool IsInDistance(Vector2<float> positionA, Vector2<float> positionB, float distance);
bool OutOfBorderX(float positionX);
bool OutOfBorderY(float positionY);

Vector2<float> GetCursorPosition();
Vector2<float>  OrientationAsVector(float orientation);
Vector2<float> RotateVector(float degree, Vector2<float> startPoint, Vector2<float> endPoint);

float RandomBinomal(float a, float b);
float VectorAsOrientation(Vector2<float> direction);

float WrapMax(float rotation, float maxValue);
float WrapMinMax(float rotation, float minValue, float maxValue);

//int BinarySearch(std::vector<std::shared_ptr<ObjectBase>> objectVector, int low, int high, int targetID);

int Partition(std::vector<float>& objectVector,  int start, int end);
void QuickSort(std::vector<float>& objectVector, int start, int end);

int Partition(std::vector<CostAndSlot>& objectVector,  int start, int end);
void QuickSort(std::vector<CostAndSlot>& objectVector, int start, int end);

int Partition(std::vector<CharacterAndSlots>& objectVector,  int start, int end);
void QuickSort(std::vector<CharacterAndSlots>& objectVector, int start, int end);