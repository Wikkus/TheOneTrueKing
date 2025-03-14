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
	Weapon,
	Count
};

class Collider;
class Timer;

class ObjectBase : public std::enable_shared_from_this<ObjectBase> {
public:
	ObjectBase(const ObjectType& objectType);
	~ObjectBase() {}

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render() {}
	virtual void RenderText() {}

	virtual const std::shared_ptr<Collider> GetCollider() const;

	virtual void SetPosition(const Vector2<float>& position);
	virtual void TakeDamage(const int& damageAmount);

	virtual void ActivateObject(const Vector2<float>& position, const Vector2<float>& direction, const float& orienation);
	virtual void DeactivateObject();
	
	const unsigned int GetObjectID() const;	
	const float GetOrientation() const;
	const float GetRotation() const;
	const ObjectType GetObjectType() const;

	const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const;
	const std::shared_ptr<ObjectBase> GetTargetObject() const;
	
	const std::shared_ptr<Sprite> GetSprite() const;

	const Vector2<float> GetDirection() const;
	const Vector2<float> GetPosition() const;
	const Vector2<float> GetTargetPosition() const;
	const Vector2<float> GetVelocity() const;	

	virtual const int GetDamage() const;

	void SetOrientation(const float& orientation);
	void SetRotation(const float& rotation);
	void SetTargetPosition(const Vector2<float>& targetPosition);
	void SetVelocity(const Vector2<float>& velocity);

	virtual void QueryObjects();

protected:
	std::vector<std::shared_ptr<ObjectBase>> _queriedObjects;
	std::vector<std::shared_ptr<ObjectBase>> _queriedProjectiles;
	std::vector<std::shared_ptr<ObjectBase>> _queriedObstacles;

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

