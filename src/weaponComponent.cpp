#include "weaponComponent.h"

#include "collision.h"
#include "dataStructuresAndMethods.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "sprite.h"
#include "timerManager.h"

SwordComponent::SwordComponent() {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load("res/sprites/Sword.png");
	_attackCooldownTimer = timerManager->CreateTimer(0.75f);
	_chargeAttackTimer = timerManager->CreateTimer(0.25f);
	_attackRange = 25.f;

	_attackDamage = 15;
	_healthModifier = 25;

	_weaponType = WeaponType::Sword;
}

void SwordComponent::Render(Vector2<float> position, float orientation) {
	_sprite->RenderWithOrientation(position, orientation);
}
//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack(Vector2<float> position, float orientation) {
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

const bool SwordComponent::GetIsAttacking() const {
	return _isAttacking;
}

const int SwordComponent::GetAttackDamage() const {
	return _attackDamage;
}

const int SwordComponent::GetHealthModifier() const {
	return _healthModifier;
}

const float SwordComponent::GetAttackRange() const {
	return _attackRange;
}

const WeaponType SwordComponent::GetWeaponType() const {
	return _weaponType;
}

StaffComponent::StaffComponent() {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load("res/sprites/Staff.png");

	_attackCooldownTimer = timerManager->CreateTimer(1.0f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);

	_attackRange = 300.f;

	_attackDamage = 10;
	_healthModifier = -25;
	
	_weaponType = WeaponType::Staff;
}

void StaffComponent::Render(Vector2<float> position, float orientation) {
	_sprite->RenderWithOrientation(position, orientation);
}
//If the weapon is a staff it shoots a fireball towards the player
void StaffComponent::Attack(Vector2<float> position, float orientation) {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		Vector2<float> direction = Vector2<float>(playerCharacter->GetPosition() - position).normalized();	
		projectileManager->SpawnProjectile(ProjectileType::EnemyProjectile, projectileManager->GetEnemyProjectileSprite(), 
			VectorAsOrientation(direction), _attackDamage, _projectileSpeed, direction, position);
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (IsInDistance(playerCharacter->GetPosition(), position, _attackRange) && !_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
}

const bool StaffComponent::GetIsAttacking() const {
	return _isAttacking;
}

const float StaffComponent::GetAttackRange() const {
	return _attackRange;
}

const int StaffComponent::GetAttackDamage() const {
	return _attackDamage;
}

const int StaffComponent::GetHealthModifier() const {
	return _healthModifier;
}

const WeaponType StaffComponent::GetWeaponType() const {
	return _weaponType;
}