#pragma once
#include "vector2.h"

struct Circle {
	float radius = 0.f;
	Vector2<float> position = { 0.f, 0.f };
};

struct Collision {
	Vector2<float> position = { 0.f, 0.f };
	Vector2<float> normal = { 0.f, 0.f };

	bool collisionDetected = false;
};

struct AABB {
	static AABB makeFromPositionSize(Vector2<float> position, float h, float w);

	void SetTargetPosition(Vector2<float> newPosition);

	float height = 0.f;
	float width = 0.f;

	Vector2<float> position = { 0.f, 0.f };
	Vector2<float> min = { 0.f, 0.f };
	Vector2<float> max = { 0.f, 0.f };
};

struct Ray {
	Vector2<float> startPosition = { 0.f, 0.f };
	Vector2<float> direction = { 0.f, 0.f };
	float length = 0.f;
};

bool CircleIntersect(Circle circleA, Circle circleB);

bool AABBIntersect(AABB& boxA, AABB& boxB);

bool AABBCircleIntersect(AABB& box, Circle& circle);