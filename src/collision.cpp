#include "collision.h"
#include "dataStructuresAndMethods.h"
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

bool CollisionCheck::AABBIntersect(AABB& boxA, AABB& boxB) {
	return (
		boxA.GetMax().x > boxB.GetMin().x &&
		boxB.GetMax().x > boxA.GetMin().x &&
		boxA.GetMax().y > boxB.GetMin().y &&
		boxB.GetMax().y > boxA.GetMin().y);
}

bool CollisionCheck::AABBCircleIntersect(AABB& box, Circle& circle) {
	_clamped = { Clamp(circle.GetPosition().x, box.GetMin().x, box.GetMax().x),
		Clamp(circle.GetPosition().y, box.GetMin().y, box.GetMax().y) };

	_delta = circle.GetPosition() - _clamped;
	return (_delta.absolute() < circle.GetRadius());
}

bool CollisionCheck::CircleIntersect(Circle circleA, Circle circleB) {
	_delta = circleB.GetPosition() - circleA.GetPosition();
	return _delta.absolute() < (circleA.GetRadius() + circleB.GetRadius());
}
