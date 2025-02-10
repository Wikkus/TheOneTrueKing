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

	const std::shared_ptr<Collider> GetCollider() const override;

	virtual bool TakeDamage(unsigned int damageAmount);

	virtual const EnemyType GetEnemyType() const;
	 
	virtual const float GetRotation() const;

	virtual const int GetCurrentHealth() const;

	virtual const int GetFormationIndex() const;

	virtual const BehaviorData GetBehaviorData() const;	

	virtual const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const;

	virtual const std::shared_ptr<WeaponComponent> GetWeaponComponent() const;
	
	virtual bool HandleAttack();
	virtual bool UpdateMovement();

	virtual void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType);
	virtual void DeactivateEnemy();
	
	virtual void SetFormationIndex(int formationIndex);
	virtual void SetTargetPosition(Vector2<float> targetPosition);
	virtual void SetTargetOrientation(float targetOrientation);

protected:
	std::shared_ptr<SlotAssignment> _currentSlotAssignment;

	SteeringOutput _steeringOutput;

	std::shared_ptr<BlendSteering> _blendSteering = nullptr;
	std::shared_ptr<PrioritySteering> _prioritySteering = nullptr;
	std::shared_ptr<WeaponComponent> _weaponComponent = nullptr;

	BehaviorAndWeight _alignBehavior;
	BehaviorAndWeight _faceBehavior;
	BehaviorData _behaviorData;

	std::shared_ptr<Circle> _circleCollider = nullptr;
	std::shared_ptr<Circle> _attackRadius = nullptr;

	const EnemyType _enemyType = EnemyType::Count;

	bool _playerInRange = false;
	bool _collidedWithPlayer = false;
	
	int _currentHealth = 0;
	int _maxHealth = 0;

	int _formationIndex = -1;

	float _rotation = 0.f;
};

