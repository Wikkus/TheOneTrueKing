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

	void HandleAttack() override;

private:
	const char* _boarSprite = "res/sprites/MadBoar.png";

	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;

	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;

	bool _damagedPlayer = false;
	bool _isAttacking = false;

	float _dashDistance = 0.f;
	float _dashSpeed = 300.f;

	int _attackDamage = 0;

	bool _playerInRange = false;
	bool _collidedWithPlayer = false;
};

