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

	void HandleAttack() override;
	
private:
	void UpdateTarget();
	const char* _humanSprite = "res/sprites/Human.png";
	unsigned int _weaponPicked = 0;
	int _numberOfWeaponTypes = 0;

	float _linearTargetRadius = 0.f;

	CurrentTarget _currentTargetState = CurrentTarget::SlotFormation;
};

