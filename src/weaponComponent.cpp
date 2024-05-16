#include "weaponComponent.h"

#include "collision.h"
#include "dataStructuresAndMethods.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "sprite.h"
#include "timerManager.h"

ShieldComponent::ShieldComponent(EnemyBase* enemyOwner) : WeaponComponent(enemyOwner) {
	_enemyOwner = enemyOwner;
	_sprite = std::make_shared<Sprite>();
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

ShieldComponent::~ShieldComponent() {
	_enemyOwner = nullptr;
	delete _enemyOwner;
}

void ShieldComponent::Render(Vector2<float> position, float orientation) {
	_sprite->RenderWithOrientation(position, orientation);
}

void ShieldComponent::Attack(Vector2<float> position, float orientation) {
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

bool ShieldComponent::AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) {
	return false;
}

const bool ShieldComponent::GetIsAttacking() const {
	return _isAttacking;
}

const float ShieldComponent::GetAttackRange() const {
	return _attackRange;
}

const int ShieldComponent::GetAttackDamage() const {
	return _attackDamage;
}

const int ShieldComponent::GetHealthModifier() const {
	return _healthModifier;
}

const WeaponType ShieldComponent::GetWeaponType() const {
	return _weaponType;
}

StaffComponent::StaffComponent(EnemyBase* enemyOwner) : WeaponComponent(enemyOwner) {
	_enemyOwner = enemyOwner;
	_sprite = std::make_shared<Sprite>();
	_sprite->Load("res/sprites/Staff.png");

	_attackCooldownTimer = timerManager->CreateTimer(1.0f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);

	_attackRange = 300.f;

	_attackDamage = 10;
	_healthModifier = -25;
	
	_weaponType = WeaponType::Staff;
}

StaffComponent::~StaffComponent() {
	_enemyOwner = nullptr;
	delete _enemyOwner;
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

bool StaffComponent::AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) {
	return false;
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

SwordComponent::SwordComponent(EnemyBase* enemyOwner) : WeaponComponent(enemyOwner) {
	_enemyOwner = enemyOwner;
	_sprite = std::make_shared<Sprite>();
	_sprite->Load("res/sprites/Sword.png");
	_attackCooldownTimer = timerManager->CreateTimer(0.75f);
	_chargeAttackTimer = timerManager->CreateTimer(0.25f);
	_attackRange = 25.f;

	_attackDamage = 15;
	_healthModifier = 25;

	_weaponType = WeaponType::Sword;
}

SwordComponent::~SwordComponent() {
	_enemyOwner = nullptr;
	delete _enemyOwner;
}

void SwordComponent::Render(Vector2<float> position, float orientation) {
	_sprite->RenderWithOrientation(position, orientation);
}
//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack(Vector2<float> position, float orientation) {
	//if ((playerCharacter->GetPosition() - position).absolute() <= 200.f) {
	//	_enemyOwner->SetTargetPosition(playerCharacter->GetPosition());
	//	
	//}
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

bool SwordComponent::AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) {
	if (isInDistance) {
		return IsInDistance(targetPosition, position, distance);
	} else {
		return IsOutOfDistance(targetPosition, position, distance);
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

