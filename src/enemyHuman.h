#pragma once
#include "collision.h"
#include "enemyBase.h"
#include "sprite.h"
#include "vector2.h"

#include <memory>

enum class CurrentTarget {
	Player,
	SlotFormation,
	Count
};

class WeaponComponent;

class EnemyHuman : public EnemyBase {
public:
	EnemyHuman(unsigned int objectID, EnemyType enemyType);
	~EnemyHuman();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	bool TakeDamage(unsigned int damageAmount) override;

	const BehaviorData GetBehaviorData() const override;
	const std::shared_ptr<Collider> GetCollider() const override;
	const EnemyType GetEnemyType() const override;
	const ObjectType GetObjectType() const override;

	const float GetOrientation() const override;
	const float GetRotation() const override;

	const int GetCurrentHealth() const override;
	const unsigned int GetObjectID() const override;
	const int GetFormationIndex() const override;

	const std::shared_ptr<Sprite> GetSprite() const override;

	const Vector2<float> GetPosition() const override;
	const Vector2<float> GetVelocity() const override;
	
	const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const override;

	const std::shared_ptr<WeaponComponent> GetWeaponComponent() const override;

	void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) override;
	void DeactivateEnemy() override;
	void HandleAttack() override;

	void SetFormationIndex(int formationIndex) override;
	void SetPosition(Vector2<float> position) override;
	void SetTargetPosition(Vector2<float> targetPosition) override;
	void SetTargetOrientation(float targetOrientation) override;
	void SetVelocity(Vector2<float> velocity) override;

private:
	void UpdateMovement();
	void UpdateTarget();
	void PickWeapon(WeaponType weaponType);
	const char* _humanSprite = "res/sprites/Human.png";
	unsigned int _weaponPicked = 0;
	int _numberOfWeaponTypes = 0;

	float _linearTargetRadius = 0.f;

	CurrentTarget _currentTarget = CurrentTarget::SlotFormation;
};

