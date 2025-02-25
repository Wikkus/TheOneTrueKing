#include "weaponComponent.h"

#include "collision.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "stateStack.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "sprite.h"
#include "timerManager.h"
#include "weaponManager.h"

WeaponComponent::WeaponComponent() : ObjectBase(ObjectType::Weapon) {
	_sprite = std::make_shared<Sprite>();
	_position = Vector2<float>(-10000.f, -10000.f);

	_maxHealth = 100;
	_currentHealth = _maxHealth;

	_collider = std::make_shared<Circle>(false);
}

void WeaponComponent::Init() {}

void WeaponComponent::Update() {
	_position = _owner->GetPosition();
	_orientation = _owner->GetOrientation();
	_direction = (_owner->GetCurrentTarget()->GetPosition() - _owner->GetPosition()).normalized();
}

void WeaponComponent::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
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

void WeaponComponent::ResetTimers() {
	_attackCooldownTimer->ResetTimer();
	_chargeAttackTimer->ResetTimer();
}

void WeaponComponent::SetOwner(std::shared_ptr<ObjectBase> owner) {
	_owner = owner;
}

void WeaponComponent::DeactivateObject() {
	_orientation = 0.f;
	_direction = { 0.f, 0.f };
	_position = { -10000.f, 10000.f };
	_rotation = 0.f;
	_collider->SetPosition(_position);
	_velocity = { 0.f, 0.f };
	DeactivateTimers();
	_owner = nullptr;
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

void ShieldComponent::Attack() {
	
}

StaffComponent::StaffComponent() {
	_sprite->Load("res/sprites/Staff.png");

	_attackCooldownTimer = timerManager->CreateTimer(1.0f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);

	_attackRange = 300.f;

	_attackDamage = 10;
	_healthModifier = -25;
	
	_weaponType = WeaponType::Staff;

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->h * 0.5f);
}

StaffComponent::~StaffComponent() {}

void StaffComponent::Init() {
	switch (_owner->GetObjectType()) {
	case ObjectType::Enemy:
		_projectileType = ProjectileType::EnemyFireball;
		if (gameStateHandler->GetGameMode() == GameMode::Formation) {
			_unlimitedRange = true;
		} else {
			_unlimitedRange = false;
		}
		break;
	case ObjectType::Player:
		_projectileType = ProjectileType::PlayerFireball;
		_unlimitedRange = true;
		break;
	default:
		break;
	}
}

//If the weapon is a staff it shoots a fireball towards the player
void StaffComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		projectileManager->SpawnProjectile(_owner, _projectileType, universalFunctions->VectorAsOrientation(_direction), 
			_direction, _position, _attackDamage, _projectileSpeed);
		
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (!_isAttacking) {	
		if (_unlimitedRange) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;

		} else if (universalFunctions->IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;
		}
	}	
}

SuperStaffComponent::SuperStaffComponent() {
	_sprite->Load("res/sprites/Staff.png");

	_attackCooldownTimer = timerManager->CreateTimer(0.25f);

	_attackDamage = 150;
	_healthModifier = 0;

	_weaponType = WeaponType::Staff;

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->h * 0.5f);
}

SuperStaffComponent::~SuperStaffComponent() {
}

void SuperStaffComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	_multiShotAngle = (-_angleOffset * _multiShotAmount);
	_multiShotAngle /= _multiShotAmount;

	for (unsigned int i = 0; i < _multiShotAmount; i++) {
		_multiShotDirection = _direction.rotated(_multiShotAngle);

		projectileManager->SpawnProjectile(_owner, _projectileType, universalFunctions->VectorAsOrientation(_direction) + _multiShotAngle,
			_multiShotDirection, _position, _attackDamage, _projectileSpeed);
		_multiShotAngle += _angleOffset;
	}
	_attackCooldownTimer->ResetTimer();
}


SwordComponent::SwordComponent() {
	_sprite->Load("res/sprites/Sword.png");
	_attackCooldownTimer = timerManager->CreateTimer(0.75f);
	_chargeAttackTimer = timerManager->CreateTimer(0.25f);
	_attackRange = 25.f;

	_attackDamage = 15;
	_healthModifier = 25;

	_weaponType = WeaponType::Sword;

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->h * 0.5f);
}

SwordComponent::~SwordComponent() {}

//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		if (universalFunctions->IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_owner->GetCurrentTarget()->TakeDamage(_attackDamage);
		}
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (!_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
}

TusksComponent::TusksComponent() {
	_sprite->Load("res/sprites/Tusks.png");
	_attackCooldownTimer = timerManager->CreateTimer(1.5f);
	_chargeAttackTimer = timerManager->CreateTimer(1.f);
	_damageCooldown = timerManager->CreateTimer(0.5f);
	_attackRange = 200.f;

	_attackDamage = 20;
	_healthModifier = 75;

	_weaponType = WeaponType::Tusks;

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->h * 0.5f);
}

TusksComponent::~TusksComponent() {}

void TusksComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking) {
		_owner->SetPosition(_owner->GetPosition() + _dashDirection * _dashSpeed * deltaTime);
		if (!_damageCooldown->GetTimerActive()) {
			for (unsigned int i = 0; i < _owner->GetQueriedObjects().size(); i++) {
				if (_owner->GetQueriedObjects()[i]->GetObjectType() == _owner->GetCurrentTarget()->GetObjectType()) {
					_owner->GetQueriedObjects()[i]->TakeDamage(_attackDamage);
					_damageCooldown->ResetTimer();
				}
			}
		}
		if ((_dashStartPosition - _owner->GetPosition()).absolute() > _dashDistance) {
			_attackCooldownTimer->ResetTimer();
			_isAttacking = false;
		}
	} else {	
		if (universalFunctions->IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->DeactivateTimer();
			_dashDirection = (_owner->GetCurrentTarget()->GetPosition() - _owner->GetPosition());
			_dashDistance = _dashDirection.absolute() + 100.f;
			_dashDirection.normalize();
			_dashStartPosition = _owner->GetPosition();
			_owner->SetVelocity({ 0.f, 0.f });
			_owner->SetRotation(0.f);
			_isAttacking = true;
		}
	}
}
