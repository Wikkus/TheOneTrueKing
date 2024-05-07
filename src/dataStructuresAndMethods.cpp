#include "dataStructuresAndMethods.h"
#include "formationManager.h"
#include "gameEngine.h"
#include "objectBase.h"


bool IsInDistance(Vector2<float> positionA, Vector2<float> positionB, float distance) {
	return Vector2<float>::distanceBetweenVectors(positionA, positionB) <= distance;
}

bool OutOfBorderX(float positionX) {
	return positionX < 0.f || positionX > windowWidth;
}

bool OutOfBorderY(float positionY) {
	return positionY < 0.f || positionY > windowHeight;
}

Vector2<float> GetCursorPosition() {
	int mousePositionX;
	int mousePositionY;
	SDL_GetMouseState(&mousePositionX, &mousePositionY);
	Vector2<float> mousePosition = Vector2<float>(mousePositionX, mousePositionY);
	return mousePosition;

}

Vector2<float> OrientationAsVector(float orientation) {
	return  Vector2<float>(-sinf(orientation), cosf(orientation)).normalized();
}

Vector2<float> RotateVector(float degree, Vector2<float> startPoint, Vector2<float> endPoint) {
	Vector2 direction = endPoint - startPoint;
	Vector2 rotatedVector =
		Vector2(direction.x * cos(degree) - direction.y * sin(degree),
			direction.x * sin(degree) + direction.y * cos(degree));
	return rotatedVector;
}

float RandomBinomal(float a, float b) {
	std::uniform_real_distribution<float> dist(a, b);
	float randomNumber = dist(randomEngine);
	return randomNumber;
}

float VectorAsOrientation(Vector2<float> direction) {
	direction.normalize();
	return atan2f(direction.x, -direction.y);
}

float WrapMax(float rotation, float maxValue) {
	return fmod(maxValue + fmod(rotation, maxValue), maxValue);
}

float WrapMinMax(float rotation, float minValue, float maxValue) {
	return minValue + WrapMax(rotation - minValue, maxValue - minValue);
}

int Partition(std::vector<float>& objectVector, int start, int end) {
	float pivot = objectVector[start];

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i] <= pivot) {
			count++;
		}
	}

	int pivotIndex = start + count;
	std::swap(objectVector[pivotIndex], objectVector[start]);

	int i = start, k = end;

	while (i < pivotIndex && k > pivotIndex) {

		while (objectVector[i] <= pivot) {
			i++;
		}

		while (objectVector[k] > pivot) {
			k--;
		}

		if (i < pivotIndex && k > pivotIndex) {
			std::swap(objectVector[i++], objectVector[k--]);
		}
	}
	return pivotIndex;
}
void QuickSort(std::vector<float>& objectVector, int start, int end) {
	if (start >= end) {
		return;
	}
	int p = Partition(objectVector, start, end);

	QuickSort(objectVector, start, p - 1);
	QuickSort(objectVector, p + 1, end);
}

int Partition(std::vector<CostAndSlot>& objectVector, int start, int end) {
	float pivot = objectVector[start].cost;

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i].cost <= pivot) {
			count++;
		}
	}

	int pivotIndex = start + count;
	std::swap(objectVector[pivotIndex], objectVector[start]);

	int i = start, k = end;

	while (i < pivotIndex && k > pivotIndex) {

		while (objectVector[i].cost <= pivot) {
			i++;
		}

		while (objectVector[k].cost > pivot) {
			k--;
		}

		if (i < pivotIndex && k > pivotIndex) {
			std::swap(objectVector[i++], objectVector[k--]);
		}
	}
	return pivotIndex;
}
void QuickSort(std::vector<CostAndSlot>& objectVector, int start, int end) {
	if (start >= end) {
		return;
	}
	int p = Partition(objectVector, start, end);

	QuickSort(objectVector, start, p - 1);
	QuickSort(objectVector, p + 1, end);
}

int Partition(std::vector<CharacterAndSlots>& objectVector, int start, int end) {
	float pivot = objectVector[start].assignmentEase;

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i].assignmentEase <= pivot) {
			count++;
		}
	}

	int pivotIndex = start + count;
	std::swap(objectVector[pivotIndex], objectVector[start]);

	int i = start, k = end;

	while (i < pivotIndex && k > pivotIndex) {

		while (objectVector[i].assignmentEase <= pivot) {
			i++;
		}

		while (objectVector[k].assignmentEase > pivot) {
			k--;
		}

		if (i < pivotIndex && k > pivotIndex) {
			std::swap(objectVector[i++], objectVector[k--]);
		}
	}
	return pivotIndex;
}
void QuickSort(std::vector<CharacterAndSlots>& objectVector, int start, int end) {
	if (start >= end) {
		return;
	}
	int p = Partition(objectVector, start, end);

	QuickSort(objectVector, start, p - 1);
	QuickSort(objectVector, p + 1, end);
}