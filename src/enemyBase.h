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

	void Init() override = 0;
	void Update() override = 0;
	void Render() override = 0;
	void RenderText() override = 0;

	const ObjectType GetObjectType() const override = 0;
	const std::shared_ptr<Sprite> GetSprite() const override = 0;
	const float GetOrientation() const override = 0;
	const unsigned int GetObjectID() const override = 0;
	const Vector2<float> GetPosition() const override = 0;

	virtual bool TakeDamage(unsigned int damageAmount) = 0;

	virtual const std::shared_ptr<Collider> GetCollider() const = 0;
	virtual const EnemyType GetEnemyType() const = 0;
	 

	virtual const float GetRotation() const = 0;

	virtual const int GetCurrentHealth() const = 0;

	virtual const int GetFormationIndex() const = 0;

	virtual const BehaviorData GetBehaviorData() const = 0;
	
	virtual const Vector2<float> GetVelocity() const = 0;

	virtual const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const = 0;

	virtual const std::shared_ptr<WeaponComponent> GetWeaponComponent() const = 0;
	
	virtual void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) = 0;
	virtual void DeactivateEnemy() = 0;
	virtual void HandleAttack() = 0;
	
	virtual void SetFormationIndex(int formationIndex) = 0;
	virtual void SetPosition(Vector2<float> position) = 0;
	virtual void SetTargetPosition(Vector2<float> targetPosition) = 0;
	virtual void SetTargetOrientation(float targetOrientation) = 0;
	virtual void SetVelocity(Vector2<float> velocity) = 0;


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

