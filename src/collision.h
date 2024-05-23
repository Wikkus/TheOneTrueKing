#pragma once
#include "vector2.h"

enum class ColliderType {
	AABB,
	Circle,
	Count
};

class Collider {
public:
	Collider() {}
	~Collider() {}

	virtual const ColliderType GetColliderType() const = 0;
	virtual const Vector2<float> GetPosition() const = 0;

	virtual void SetPosition(Vector2<float> position) = 0;

protected:
	ColliderType _colliderType = ColliderType::Count;

	Vector2<float> _position = { 0.f, 0.f };
};

class AABB : public Collider {
public:
	AABB();
	~AABB() {}

	void Init(Vector2<float> position, float h, float w);

	const ColliderType GetColliderType() const override;
	const Vector2<float> GetPosition() const override;
	
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
	Circle();
	~Circle() {}

	void Init(Vector2<float> position, float radius);

	const ColliderType GetColliderType() const override;

	const float GetRadius() const;
	const Vector2<float> GetPosition() const override;

	void SetPosition(Vector2<float> position) override;

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

bool ColliderIntersect(std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB);

bool CircleIntersect(Circle circleA, Circle circleB);

bool AABBIntersect(AABB& boxA, AABB& boxB);

bool AABBCircleIntersect(AABB& box, Circle& circle);