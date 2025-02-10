#pragma once
#include "sprite.h"
#include "vector2.h"


enum class ObjectType {
	Enemy,
	Player,
	Projectile,
	Obstacle,
	Count
};

class Collider;
class Timer;

class ObjectBase {
public:
	ObjectBase(unsigned int objectID, ObjectType objectType) : _objectID(objectID), _objectType(objectType) {}
	~ObjectBase() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void RenderText() {}

	virtual const unsigned int GetObjectID() const;	
	virtual const std::shared_ptr<Collider> GetCollider() const;
	virtual const float GetOrientation() const;
	virtual const ObjectType GetObjectType() const;
	
	const std::shared_ptr<Sprite> GetSprite() const;

	virtual const Vector2<float> GetPosition() const;
	virtual const Vector2<float> GetVelocity() const;

	virtual void SetOrientation(float orientation);
	virtual void SetPosition(Vector2<float> position);
	virtual void SetVelocity(Vector2<float> velocity);


protected:
	float _orientation = 0.f;	
	const unsigned int _objectID;

	const ObjectType _objectType = ObjectType::Count;	
	
	std::shared_ptr<Sprite> _sprite = nullptr;

	Vector2<float> _direction = { 0.f, 0.f };
	Vector2<float> _position = { -10000.f, -10000.f };
	Vector2<float> _velocity = { 0.f, 0.f };
};

