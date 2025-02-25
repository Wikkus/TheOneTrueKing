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
	Collider(bool isActive);
	~Collider() {}


	const bool GetIsActive() const;
	const ColliderType GetColliderType() const;
	const Vector2<float> GetPosition() const;

	virtual void SetPosition(Vector2<float> position);
	virtual void SetIsActive(bool isActive);

protected:
	std::shared_ptr<ObjectBase> _owner = nullptr;
	ColliderType _colliderType = ColliderType::Count;
	bool _isActive = false;
	Vector2<float> _position = { 0.f, 0.f };
};

class AABB : public Collider {
public:
	AABB(bool isActive);
	~AABB() {}

	void Init(Vector2<float> position, float h, float w);

	void SetPosition(Vector2<float> position) override;

	const float GetHeight() const;
	const float GetWidth() const;
	const Vector2<float> GetMax() const;
	const Vector2<float> GetMin() const;

	void SetHeight(float height);
	void SetWidth(float width);

private:
	float _height = 0.f;
	float _width = 0.f;

	Vector2<float> _min = { 0.f, 0.f };
	Vector2<float> _max = { 0.f, 0.f };
};

class Circle : public Collider {
public:
	Circle(bool isActive);
	~Circle() {}

	void Init(Vector2<float> position, float radius);

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
	bool AABBIntersect(AABB& boxA, AABB& boxB);

	bool AABBCircleIntersect(AABB& box, Circle& circle);

	bool CircleIntersect(Circle circleA, Circle circleB);

private:
	Vector2<float> _clamped = { 0.f, 0.f };
	Vector2<float> _delta = { 0.f, 0.f };
	float distance = 0.f;

};