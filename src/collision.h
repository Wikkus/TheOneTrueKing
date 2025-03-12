#pragma once
#include "vector2.h"

class ObjectBase;

enum class ColliderType {
	AABB,
	Circle,
	Count
};

class Collider {
public:
	Collider();
	~Collider() {}


	const bool GetIsActive() const;
	const ColliderType GetColliderType() const;
	const Vector2<float> GetPosition() const;

	virtual void SetPosition(const Vector2<float>& position);
	virtual void SetIsActive(const bool& isActive);

protected:
	ColliderType _colliderType = ColliderType::Count;
	bool _isActive = true;
	Vector2<float> _position = { 0.f, 0.f };
};

class AABB : public Collider {
public:
	AABB();
	~AABB() {}

	void Init(const Vector2<float>& position, const float& h, const float& w);

	void SetPosition(const Vector2<float>& position) override;

	const float GetHeight() const;
	const float GetWidth() const;
	const Vector2<float> GetMax() const;
	const Vector2<float> GetMin() const;

	void SetHeight(const float& height);
	void SetWidth(const float& width);

private:
	float _height = 0.f;
	float _width = 0.f;

	Vector2<float> _min = { 0.f, 0.f };
	Vector2<float> _max = { 0.f, 0.f };
};

class Circle : public Collider {
public:
	Circle();
	~Circle() {}

	void Init(const Vector2<float>& position, const float& radius);

	const float GetRadius() const;

private:
	float _radius = 0.f;
};

struct Ray {
	Vector2<float> startPosition = { 0.f, 0.f };
	Vector2<float> direction = { 0.f, 0.f };
	float length = 0.f;
};

struct Collision {
	Vector2<float> position = { 0.f, 0.f };
	Vector2<float> normal = { 0.f, 0.f };

	bool collisionDetected = false;
};

class CollisionCheck {
public:
	bool AABBIntersect(const AABB& boxA, const AABB& boxB);

	bool AABBCircleIntersect(const AABB& box, const Circle& circle);

	bool CircleIntersect(const Circle& circleA, const Circle& circleB);

	bool RayAABBIntersect(const Ray& line, const AABB& box);

private:
	Vector2<float> _clamped = { 0.f, 0.f };
	Vector2<float> _delta = { 0.f, 0.f };
	float distance = 0.f;

};