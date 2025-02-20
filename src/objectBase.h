#pragma once
#include "sprite.h"
#include "vector2.h"

#include <memory>
#include <vector>


enum class ObjectType {
	Enemy,
	Player,
	Projectile,
	Obstacle,
	Count
};

class Collider;
class Timer;

class ObjectBase : public std::enable_shared_from_this<ObjectBase> {
public:
	ObjectBase(unsigned int objectID, ObjectType objectType) : _objectID(objectID), _objectType(objectType) {}
	~ObjectBase() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void RenderText() {}

	virtual const std::shared_ptr<Collider> GetCollider() const;

	virtual void SetPosition(Vector2<float> position);
	virtual void TakeDamage(unsigned int damageAmount);
	
	const unsigned int GetObjectID() const;	
	const float GetOrientation() const;
	const float GetRotation() const;
	const ObjectType GetObjectType() const;

	const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const;
	const std::shared_ptr<ObjectBase> GetCurrentTarget() const;
	
	const std::shared_ptr<Sprite> GetSprite() const;

	const Vector2<float> GetDirection() const;
	const Vector2<float> GetPosition() const;
	const Vector2<float> GetTargetPosition() const;
	const Vector2<float> GetVelocity() const;	

	void SetOrientation(float orientation);
	void SetRotation(float rotation);
	void SetTargetPosition(Vector2<float> targetPosition);
	void SetVelocity(Vector2<float> velocity);

	void QueryObjects();

protected:
	std::vector<std::shared_ptr<ObjectBase>> _queriedObjects;

	std::shared_ptr<Collider> _collider = nullptr;

	std::shared_ptr<ObjectBase> _currentTarget = nullptr;

	float _orientation = 0.f;
	float _rotation = 0.f;

	const unsigned int _objectID;

	int _currentHealth = 0;
	int _maxHealth = 0;

	const ObjectType _objectType = ObjectType::Count;

	std::shared_ptr<Sprite> _sprite = nullptr;

	Vector2<float> _direction = { 0.f, 0.f };
	Vector2<float> _position = { -10000.f, -10000.f };
	Vector2<float> _targetPosition = { -10000.f, -10000.f };
	Vector2<float> _velocity = { 0.f, 0.f };
};

