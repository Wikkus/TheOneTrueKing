#pragma once
#include "sprite.h"
#include "vector2.h"

class Timer;

enum class ObjectType {
	Enemy,
	Player,
	Projectile,
	Obstacle,
	Count
};

class Collider;

class ObjectBase {
public:
	ObjectBase(unsigned int objectID, ObjectType objectType) : _objectID(objectID), _objectType(objectType) {}
	~ObjectBase() {}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void RenderText() = 0;

	virtual const unsigned int GetObjectID() const = 0;	
	virtual const std::shared_ptr<Collider> GetCollider() const = 0;
	virtual const ObjectType GetObjectType() const = 0;

	virtual const std::shared_ptr<Sprite> GetSprite() const = 0;
	virtual const Vector2<float> GetPosition() const = 0;
	virtual const Vector2<float> GetVelocity() const = 0;


protected:
	float _orientation = 0.f;	
	const unsigned int _objectID;

	const ObjectType _objectType = ObjectType::Count;	
	
	std::shared_ptr<Sprite> _sprite = nullptr;

	Vector2<float> _direction = { 0.f, 0.f };
	Vector2<float> _position = { -10000.f, -10000.f };
	Vector2<float> _velocity = { 0.f, 0.f };
};

