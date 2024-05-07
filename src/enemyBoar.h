#pragma once
#include "collision.h"
#include "enemyBase.h"
#include "sprite.h"
#include "vector2.h"

#include <memory>

class EnemyBoar : public EnemyBase {
public:
	EnemyBoar(unsigned int objectID, EnemyType enemyType);
	~EnemyBoar();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	bool TakeDamage(unsigned int damageAmount) override;
	
	const Circle GetCollider() const override;
	const EnemyType GetEnemyType() const override;
	const ObjectType GetObjectType() const override;

	const float GetOrientation() const override;
	const float GetRotation() const override;
	
	const int GetCurrentHealth() const override;
	const unsigned int GetObjectID() const override;
	
	const std::shared_ptr<Sprite> GetSprite() const override;
	
	const BehaviorData GetBehaviorData() const override;

	const Vector2<float> GetPosition() const override;
	const Vector2<float> GetVelocity() const override;
	
	const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const override;

	const std::shared_ptr<WeaponComponent> GetWeaponComponent() const override;

	void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position) override;
	void DeactivateEnemy() override;
	void HandleAttack() override;

	void SetPosition(Vector2<float> position) override;
	void SetTargetPosition(Vector2<float> targetPosition) override;
	void SetTargetOrientation(float targetOrientation) override;
	void SetVelocity(Vector2<float> velocity) override;

private:
	void UpdateMovement();
	
	const char* _boarSprite = "res/sprites/MadBoar.png";

	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;

	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;

	bool _damagedPlayer = false;
	bool _isAttacking = false;

	float _dashDistance = 0.f;
	float _dashSpeed = 300.f;

	float _attackRange = 0.f;
	int _attackDamage = 0;
};

