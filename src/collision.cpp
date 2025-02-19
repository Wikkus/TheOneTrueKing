#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "objectBase.h"

#include <minmax.h>
#include <SDL2/SDL.h>

const ColliderType Collider::GetColliderType() const {
	return _colliderType;
}

const Vector2<float> Collider::GetPosition() const {
	return _position;
}

void Collider::SetPosition(Vector2<float> position) {
	_position = position;
}

AABB::AABB() {
	_colliderType = ColliderType::AABB;
}

void AABB::Init(Vector2<float> position, float h, float w) {
	_position = position;
	_min.x = position.x - (w * 0.5f);
	_min.y = position.y - (h * 0.5f);
	_max.x = position.x + (w * 0.5f);
	_max.y = position.y + (h * 0.5f);
	_height = h;
	_width = w;
}

const float AABB::GetHeight() const {
	return _height;
}

const float AABB::GetWidth() const {
	return _width;
}

const Vector2<float> AABB::GetMax() const {
	return _max;
}

const Vector2<float> AABB::GetMin() const {
	return _min;
}

void AABB::SetHeight(float height) {
	_height = height;
}

void AABB::SetWidth(float width) {
	_width = width;
}

bool ColliderIntersect(std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB) {
	switch (colliderA->GetColliderType()) {
	case ColliderType::AABB:
		switch (colliderB->GetColliderType()) {
		case ColliderType::AABB:
			return AABBIntersect(*std::static_pointer_cast<AABB>(colliderA), 
				*std::static_pointer_cast<AABB>(colliderB));
			break;
		case ColliderType::Circle:
			return AABBCircleIntersect(*std::static_pointer_cast<AABB>(colliderA), 
				*std::static_pointer_cast<Circle>(colliderB));
			break;
		default:
			break;
		}
		break;
	case ColliderType::Circle:
		switch (colliderB->GetColliderType()) {
		case ColliderType::AABB:
			return AABBCircleIntersect(*std::static_pointer_cast<AABB>(colliderB),
				*std::static_pointer_cast<Circle>(colliderA));
			break;
		case ColliderType::Circle:
			return CircleIntersect(*std::static_pointer_cast<Circle>(colliderA),
				*std::static_pointer_cast<Circle>(colliderB));
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return false;
}

bool CircleIntersect(Circle circleA, Circle circleB) {
	float dx = circleB.GetPosition().x - circleA.GetPosition().x;
	float dy = circleB.GetPosition().y - circleA.GetPosition().y;

	float distanceSquared = dx * dx + dy * dy;
	float distance = sqrt(distanceSquared);

	float radiusSum = circleA.GetRadius() + circleB.GetRadius();
	return distance < radiusSum;
}

bool AABBIntersect(AABB& boxA, AABB& boxB) {
	return (
		boxA.GetMax().x > boxB.GetMin().x &&
		boxB.GetMax().x > boxA.GetMin().x &&
		boxA.GetMax().y > boxB.GetMin().y &&
		boxB.GetMax().y > boxA.GetMin().y);
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
	float clampedX = Clamp(circle.GetPosition().x, box.GetMin().x, box.GetMax().x);
	float clampedY = Clamp(circle.GetPosition().y, box.GetMin().y, box.GetMax().y);

	float deltaX = circle.GetPosition().x - clampedX;
	float deltaY = circle.GetPosition().y - clampedY;

	float distanceSquared = deltaX * deltaX + deltaY * deltaY;
	float distance = sqrt(distanceSquared);
	return (distance < circle.GetRadius());
}

void AABB::SetPosition(Vector2<float> newPosition) {
	_position = newPosition;
	_min.x = _position.x - (_width * 0.5f);
	_min.y = _position.y - (_height * 0.5f);
	_max.x = _position.x + (_width * 0.5f);
	_max.y = _position.y + (_height * 0.5f);
}

Circle::Circle() {
	_colliderType = ColliderType::Circle;
}

void Circle::Init(Vector2<float> position, float radius) {
	_position = position;
	_radius = radius;
}

const float Circle::GetRadius() const {
	return _radius;
}

