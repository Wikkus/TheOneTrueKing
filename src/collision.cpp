#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"

#include <minmax.h>
#include <SDL2/SDL.h>

AABB AABB::makeFromPositionSize(Vector2<float> position, float h, float w) {
	AABB boxCollider;
	boxCollider.position = position;
	boxCollider.min.x = position.x - (w * 0.5f);
	boxCollider.min.y = position.y - (h * 0.5f);
	boxCollider.max.x = position.x + (w * 0.5f);
	boxCollider.max.y = position.y + (h * 0.5f);
	boxCollider.height = h;
	boxCollider.width = w;
	return boxCollider;
}

bool CircleIntersect(Circle circleA, Circle circleB) {
	float dx = circleB.position.x - circleA.position.x;
	float dy = circleB.position.y - circleA.position.y;

	float distanceSquared = dx * dx + dy * dy;
	float distance = sqrt(distanceSquared);

	float radiusSum = circleA.radius + circleB.radius;
	return distance < radiusSum;
}

bool AABBIntersect(AABB& boxA, AABB& boxB) {
	return (
		boxA.max.x > boxB.min.x &&
		boxB.max.x > boxA.min.x &&
		boxA.max.y > boxB.min.y &&
		boxB.max.y > boxA.min.y);
}


float Clamp(float a, float min, float max) {
	if (a < min) {
		return min;
	}
	else if (a > max) {
		return max;
	}
	return a;
}

bool AABBCircleIntersect(AABB& box, Circle& circle) {
	float clampedX = Clamp(circle.position.x, box.min.x, box.max.x);
	float clampedY = Clamp(circle.position.y, box.min.y, box.max.y);

	float deltaX = circle.position.x - clampedX;
	float deltaY = circle.position.y - clampedY;

	float distanceSquared = deltaX * deltaX + deltaY * deltaY;
	float distance = sqrt(distanceSquared);
	return (distance < circle.radius);
}

void AABB::SetTargetPosition(Vector2<float> newPosition) {
	position = newPosition;
	min.x = position.x - (width * 0.5f);
	min.y = position.y - (height * 0.5f);
	max.x = position.x + (width * 0.5f);
	max.y = position.y + (height * 0.5f);
}
