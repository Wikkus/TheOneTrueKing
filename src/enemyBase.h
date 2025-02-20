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
	EnemyBase(int objectID, EnemyType enemyType);
	~EnemyBase() {}

	const std::shared_ptr<Collider> GetCollider() const override;
	virtual const EnemyType GetEnemyType() const;
	 
	void TakeDamage(unsigned int damageAmount) override;

	virtual const float GetRotation() const;

	virtual const int GetCurrentHealth() const;

	virtual const int GetFormationIndex() const;

	virtual const BehaviorData GetBehaviorData() const;

	virtual const std::shared_ptr<WeaponComponent> GetWeaponComponent() const;

	virtual void HandleAttack();

	void UpdateMovement();
	void UpdateAngularMovement();
	void UpdateLinearMovement();

	virtual void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType);
	virtual void DeactivateEnemy();

	virtual void SetFormationIndex(int formationIndex);
	virtual void SetTargetOrientation(float targetOrientation);

	void SetPosition(Vector2<float> position) override;
	
	bool ReplaceSteeringBehavior(SteeringBehaviorType oldBehavior, BehaviorAndWeight newBehavior);


protected:
	std::shared_ptr<SlotAssignment> _currentSlotAssignment;

	SteeringOutput _steeringOutput;

	std::shared_ptr<BlendSteering> _blendSteering = nullptr;
	std::shared_ptr<PrioritySteering> _prioritySteering = nullptr;
	std::shared_ptr<WeaponComponent> _weaponComponent = nullptr;

	BehaviorAndWeight _alignBehavior;
	BehaviorAndWeight _faceBehavior;
	BehaviorData _behaviorData;

	const EnemyType _enemyType = EnemyType::Count;

	int _formationIndex = -1;
};

