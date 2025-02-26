#pragma once
#include "collision.h"
#include "enemyManager.h"
#include "formationHandler.h"
#include "objectBase.h"
#include "sprite.h"
#include "steeringBehavior.h"
#include "vector2.h"

class Timer;
class WeaponComponent;

enum class EnemyType {
	Boss,
	Boar,
	Human,
	Count
};

class EnemyBase : public ObjectBase {
public:
	EnemyBase(const EnemyType& enemyType);
	~EnemyBase() {}

	void Render() override;

	const std::shared_ptr<Collider> GetCollider() const override;
	const int GetDamage() const override;

	virtual const EnemyType GetEnemyType() const;
	 
	void TakeDamage(const int& damageAmount) override;

	virtual const float GetRotation() const;

	virtual const int GetCurrentHealth() const;

	virtual const int GetFormationIndex() const;

	virtual const BehaviorData GetBehaviorData() const;

	virtual const std::shared_ptr<WeaponComponent> GetWeaponComponent() const;

	void UpdateMovement();
	void UpdateAngularMovement();
	void UpdateLinearMovement();

	virtual void ActivateEnemy(const float& orienation, const Vector2<float>& direction, 
		const Vector2<float>& position, const WeaponType& weaponType);
	void DeactivateObject() override;

	virtual void SetFormationIndex(const int& formationIndex);
	virtual void SetTargetOrientation(const float& targetOrientation);

	void SetPosition(const Vector2<float>& position) override;
	
	bool ReplaceSteeringBehavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior);

protected:
	std::shared_ptr<SlotAssignment> _currentSlotAssignment = nullptr;

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

