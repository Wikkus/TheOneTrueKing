#include "weaponComponent.h"

#include "collision.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "stateStack.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "sprite.h"
#include "timerHandler.h"
#include "weaponManager.h"

WeaponComponent::WeaponComponent() : ObjectBase(ObjectType::Weapon) {
	_sprite = std::make_shared<Sprite>();
	_position = deactivatedPosition;

	_maxHealth = 100;
	_currentHealth = _maxHealth;

	_collider = std::make_shared<Circle>();
	_collider->SetIsActive(false);	
	
	_attackCooldownTimer = timerHandler->SpawnTimer(1.f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(0.5f, false, false);
	_isAttacking = false;
}

void WeaponComponent::Init() {}

void WeaponComponent::Update() {
	_position = _owner->GetPosition();
	_orientation = _owner->GetOrientation();
	_direction = (_owner->GetTargetObject()->GetPosition() - _owner->GetPosition()).normalized();
}

void WeaponComponent::Render() {
	if (_renderWeapon) {
		_sprite->RenderWithOrientation(0, _position, _orientation);
	}
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
	_attackCooldownTimer->SetTimer(false, false);
	_chargeAttackTimer->SetTimer(false, false);
}

void WeaponComponent::ResetTimers() {
	_attackCooldownTimer->ResetTimer();
	_chargeAttackTimer->ResetTimer();
}

void WeaponComponent::SetOwner(std::shared_ptr<ObjectBase> owner, const bool& renderWeapon) {
	_owner = owner;
	_renderWeapon = renderWeapon;
}

void WeaponComponent::DeactivateObject() {
	_position = deactivatedPosition;
	_collider->SetPosition(_position);
	DeactivateTimers();
	_owner = nullptr;
}

ShieldComponent::ShieldComponent() {
	_sprite->Load(_path);

	_attackCooldownTimer = timerHandler->SpawnTimer(1.f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(0.5f, false, false);

	_attackRange = 25.f;

	_attackDamage = 10;
	_healthModifier = 125;

	_weaponType = WeaponType::Shield;
}

ShieldComponent::~ShieldComponent() {}

void ShieldComponent::Attack() {
	
}

StaffComponent::StaffComponent() {
	_sprite->Load(_path);

	_attackRange = 300.f;

	_attackDamage = 10;
	_healthModifier = -25;
	
	_weaponType = WeaponType::Staff;
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
	if (_attackCooldownTimer->GetIsActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetIsFinished()) {
		projectileManager->SpawnProjectile(_owner, _projectileType, universalFunctions->VectorAsOrientation(_direction), 
			_direction, _position, _attackDamage, _projectileSpeed);
		
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (!_isAttacking) {	
		if (_unlimitedRange) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;

		} else if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;
		}
	}	
}

SuperStaffComponent::SuperStaffComponent() {
	_sprite->Load(_path);

	_attackCooldownTimer = timerHandler->SpawnTimer(0.25f, false, false);

	_attackDamage = 150;
	_healthModifier = 0;

	_weaponType = WeaponType::Staff;
}

SuperStaffComponent::~SuperStaffComponent() {
}

void SuperStaffComponent::Attack() {
	if (_attackCooldownTimer->GetIsActive()) {
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
	_sprite->Load(_path);
	_attackCooldownTimer = timerHandler->SpawnTimer(0.75f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(0.25f, false, false);

	_attackRange = 25.f;

	_attackDamage = 15;
	_healthModifier = 25;

	_weaponType = WeaponType::Sword;
}

SwordComponent::~SwordComponent() {}

//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack() {
	if (_attackCooldownTimer->GetIsActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetIsFinished()) {
		if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_owner->GetTargetObject()->TakeDamage(_attackDamage);
		}
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (!_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
}

TusksComponent::TusksComponent() {
	_sprite->Load(_path);
	_attackCooldownTimer = timerHandler->SpawnTimer(1.5f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(1.f, false, false);
	_damageCooldown = timerHandler->SpawnTimer(0.5f, false, false);
	_attackRange = 200.f;

	_attackDamage = 20;
	_healthModifier = 75;

	_weaponType = WeaponType::Tusks;
}

TusksComponent::~TusksComponent() {}

void TusksComponent::Attack() {
	if (_attackCooldownTimer->GetIsActive()) {
		return;

	} else if (_chargeAttackTimer->GetIsActive()) {
		_dashDirection = (_owner->GetTargetObject()->GetPosition() - _owner->GetPosition());
		_dashDistance = _dashDirection.absolute() + 100.f;
		_dashDirection.normalize();
		_dashStartPosition = _owner->GetPosition();
		return;
	}
	if (_isAttacking) {
		_owner->SetPosition(_owner->GetPosition() + _dashDirection * _dashSpeed * deltaTime);
		if (!_damageCooldown->GetIsActive()) {
			for (unsigned int i = 0; i < _owner->GetQueriedObjects().size(); i++) {
				if (_owner->GetQueriedObjects()[i]->GetObjectType() == _owner->GetTargetObject()->GetObjectType()) {
					_owner->GetQueriedObjects()[i]->TakeDamage(_attackDamage);
					_damageCooldown->ResetTimer();
				}
			}
		}
		if ((_dashStartPosition - _owner->GetPosition()).absolute() > _dashDistance) {
			_attackCooldownTimer->ResetTimer();
			_chargeAttackTimer->SetTimer(false, false);
			_isAttacking = false;
		}
	} else {
		if(_chargeAttackTimer->GetIsFinished()) {
			_isAttacking = true;	
		
		} else if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->ResetTimer();
			_owner->SetVelocity({ 0.f, 0.f });
			_owner->SetRotation(0.f);
		}
	}
}
