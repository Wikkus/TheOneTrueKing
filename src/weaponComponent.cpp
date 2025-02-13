#include "weaponComponent.h"

#include "collision.h"
#include "dataStructuresAndMethods.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "stateStack.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "sprite.h"
#include "timerManager.h"

WeaponComponent::WeaponComponent() {
	_sprite = std::make_shared<Sprite>();
}

void WeaponComponent::Init() {}

void WeaponComponent::Update() {}

void WeaponComponent::Render(Vector2<float> position, float orientation) {
	_sprite->RenderWithOrientation(position, orientation);
}
const bool WeaponComponent::GetIsAttacking() const {
	return _isAttacking;
}
const float WeaponComponent::GetAttackRange() const {
	return _attackRange;
}
const int WeaponComponent::GetAttackDamage() const {
	return _attackDamage;
}
const int WeaponComponent::GetHealthModifier() const {
	return _healthModifier;
}
const WeaponType WeaponComponent::GetWeaponType() const {
	return _weaponType;
}
void WeaponComponent::DeactivateTimers() {
	_attackCooldownTimer->DeactivateTimer();
	_chargeAttackTimer->DeactivateTimer();
}


ShieldComponent::ShieldComponent() {
	_sprite->Load("res/sprites/Shield.png");

	_attackCooldownTimer = timerManager->CreateTimer(1.0f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);

	_attackCooldownTimer->DeactivateTimer();
	_chargeAttackTimer->DeactivateTimer();

	_attackRange = 25.f;

	_attackDamage = 10;
	_healthModifier = 125;

	_weaponType = WeaponType::Shield;
}

ShieldComponent::~ShieldComponent() {}

void ShieldComponent::Attack(Vector2<float> position) {
	//if (_attackCooldownTimer->GetTimerActive()) {
	//	return;
	//}
	//if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
	//	_isAttacking = false;
	//	_attackCooldownTimer->ResetTimer();

	//} else if (IsInDistance(playerCharacter->GetPosition(), position, _attackRange) && !_isAttacking) {
	//	_chargeAttackTimer->ResetTimer();
	//	_isAttacking = true;
	//}
}
StaffComponent::StaffComponent() {
	_sprite->Load("res/sprites/Staff.png");

	_attackCooldownTimer = timerManager->CreateTimer(1.0f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);

	_attackRange = 300.f;

	_attackDamage = 10;
	_healthModifier = -25;
	
	_weaponType = WeaponType::Staff;
}

StaffComponent::~StaffComponent() {}

//If the weapon is a staff it shoots a fireball towards the player
void StaffComponent::Attack(Vector2<float> position) {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		_projectileDirection = Vector2<float>(playerCharacter->GetPosition() - position).normalized();
		_projectileOrientation = VectorAsOrientation(_projectileDirection);

		projectileManager->SpawnProjectile(ProjectileType::EnemyFireball, _projectileOrientation, _projectileDirection, position, _attackDamage, _projectileSpeed);
		
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if ((!_isAttacking && gameStateHandler->GetGameMode() == GameMode::Formation) ||
		(!_isAttacking && IsInDistance(playerCharacter->GetPosition(), position, _attackRange))) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
}

SwordComponent::SwordComponent() {
	_sprite->Load("res/sprites/Sword.png");
	_attackCooldownTimer = timerManager->CreateTimer(0.75f);
	_chargeAttackTimer = timerManager->CreateTimer(0.25f);
	_attackRange = 25.f;

	_attackDamage = 15;
	_healthModifier = 25;

	_weaponType = WeaponType::Sword;
}

SwordComponent::~SwordComponent() {}

//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack(Vector2<float> position) {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		if (IsInDistance(playerCharacter->GetPosition(), position, _attackRange)) {
			playerCharacter->TakeDamage(_attackDamage);
		}
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (IsInDistance(playerCharacter->GetPosition(), position, _attackRange) && !_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
}
