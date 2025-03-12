#include "collision.h"
#include "universalFunctions.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "objectBase.h"

#include <minmax.h>
#include <SDL2/SDL.h>

Collider::Collider() {}

const bool Collider::GetIsActive() const {
	return _isActive;
}

const ColliderType Collider::GetColliderType() const {
	return _colliderType;
}

const Vector2<float> Collider::GetPosition() const {
	return _position;
}

void Collider::SetPosition(const Vector2<float>& position) {
	_position = position;
}

void Collider::SetIsActive(const bool& isActive) {
	_isActive = isActive;
}

AABB::AABB() {
	_colliderType = ColliderType::AABB;
}

void AABB::Init(const Vector2<float>& position, const float& h, const float& w) {
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

void AABB::SetHeight(const float& height) {
	_height = height;
}

void AABB::SetWidth(const float& width) {
	_width = width;
}

void AABB::SetPosition(const Vector2<float>& position) {
	_position = position;
	_min = { _position.x - (_width * 0.5f),  _position.y - (_height * 0.5f) };
	_max = { _position.x + (_width * 0.5f), _position.y + (_height * 0.5f) };
}

Circle::Circle() {
	_colliderType = ColliderType::Circle;
}

void Circle::Init(const Vector2<float>& position, const float& radius) {
	_position = position;
	_radius = radius;
}

const float Circle::GetRadius() const {
	return _radius;
}

bool CollisionCheck::AABBIntersect(const AABB& boxA, const AABB& boxB) {
	return (
		boxA.GetMax().x > boxB.GetMin().x &&
		boxB.GetMax().x > boxA.GetMin().x &&
		boxA.GetMax().y > boxB.GetMin().y &&
		boxB.GetMax().y > boxA.GetMin().y);
}

bool CollisionCheck::AABBCircleIntersect(const AABB& box, const Circle& circle) {
	_clamped = { universalFunctions->Clamp(circle.GetPosition().x, box.GetMin().x, box.GetMax().x),
		universalFunctions->Clamp(circle.GetPosition().y, box.GetMin().y, box.GetMax().y) };

	_delta = circle.GetPosition() - _clamped;
	return (_delta.absolute() < circle.GetRadius());
}

bool CollisionCheck::CircleIntersect(const Circle& circleA, const Circle& circleB) {
	_delta = circleB.GetPosition() - circleA.GetPosition();
	return _delta.absolute() < (circleA.GetRadius() + circleB.GetRadius());
}

bool CollisionCheck::RayAABBIntersect(const Ray& line, const AABB& box) {



	return false;
}
