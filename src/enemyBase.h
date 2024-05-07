#pragma once
#include "collision.h"
#include "enemyManager.h"
#include "formationManager.h"
#include "objectBase.h"
#include "sprite.h"
#include "steeringBehavior.h"
#include "vector2.h"

class Timer;
class WeaponComponent;

class EnemyBase : public ObjectBase {
public:
	EnemyBase(int objectID, EnemyType enemyType) : ObjectBase(objectID, ObjectType::Enemy), _enemyType(enemyType) {}
	~EnemyBase() {}

	virtual void Init() override = 0;
	virtual void Update() override = 0;
	virtual void Render() override = 0;
	virtual void RenderText() = 0;

	virtual const ObjectType GetObjectType() const override = 0;
	virtual const std::shared_ptr<Sprite> GetSprite() const override = 0;
	virtual const unsigned int GetObjectID() const override = 0;
	virtual const Vector2<float> GetPosition() const = 0;

	virtual bool TakeDamage(unsigned int damageAmount) = 0;

	virtual const Circle GetCollider() const = 0;
	virtual const EnemyType GetEnemyType() const = 0;
	 
	virtual const float GetOrientation() const = 0;

	virtual const float GetRotation() const = 0;

	virtual const int GetCurrentHealth() const = 0;

	virtual const BehaviorData GetBehaviorData() const = 0;
	
	virtual const Vector2<float> GetVelocity() const = 0;

	virtual const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const = 0;

	virtual const std::shared_ptr<WeaponComponent> GetWeaponComponent() const = 0;
	
	virtual void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position) = 0;
	virtual void DeactivateEnemy() = 0;
	virtual void HandleAttack() = 0;
	
	virtual void SetPosition(Vector2<float> position) = 0;
	virtual void SetTargetPosition(Vector2<float> targetPosition) = 0;
	virtual void SetTargetOrientation(float targetOrientation) = 0;
	virtual void SetVelocity(Vector2<float> velocity) = 0;

protected:
	BehaviorData _behaviorData;
	SteeringOutput _steeringOutput;
	std::shared_ptr<BlendSteering> _blendSteering = nullptr;
	std::shared_ptr<PrioritySteering> _prioritySteering = nullptr;
	std::shared_ptr<WeaponComponent> _weaponComponent = nullptr;

	Circle _circleCollider;
	const EnemyType _enemyType = EnemyType::Count;

	
	int _currentHealth = 0;
	int _maxHealth = 0;

	float _rotation = 0.f;
	
	Vector2<float> _direction = Vector2<float>(0.f, 0.f);
	Vector2<float> _velocity = Vector2<float>(0.f, 0.f);

	std::vector<std::shared_ptr<ObjectBase>> _queriedObjects;
};

