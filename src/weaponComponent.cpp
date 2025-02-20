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

void WeaponComponent::Init(std::shared_ptr<ObjectBase> owner) {
	_owner = owner;
}

void WeaponComponent::Update() {}

void WeaponComponent::Render() {
	_sprite->RenderWithOrientation(_owner->GetPosition(), _owner->GetOrientation());
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

void ShieldComponent::Attack() {
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

void StaffComponent::Init(std::shared_ptr<ObjectBase> owner) {
	_owner = owner;
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
		_projectileDirection = Vector2<float>(_owner->GetCurrentTarget()->GetPosition() - _owner->GetPosition()).normalized();
		_projectileOrientation = VectorAsOrientation(_projectileDirection);

		projectileManager->SpawnProjectile(_owner, _projectileType,
			_projectileOrientation, _projectileDirection, _owner->GetPosition(), _attackDamage, _projectileSpeed);
		
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();

	} else if (!_isAttacking) {	
		if (_unlimitedRange) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;

		} else if (IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
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

}

SuperStaffComponent::~SuperStaffComponent() {
}

void SuperStaffComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	_projectileDirection = _owner->GetTargetPosition() - _owner->GetPosition();
	_multiShotAngle = (-_angleOffset * _multiShotAmount);
	_multiShotAngle *= 0.5f;

	for (unsigned int i = 0; i < _multiShotAmount; i++) {
		_multiShotDirection = RotateDirection(_multiShotAngle, _projectileDirection);

		projectileManager->SpawnProjectile(_owner, _projectileType, VectorAsOrientation(_projectileDirection) + _multiShotAngle,
			_multiShotDirection, _owner->GetPosition(), _attackDamage, _projectileSpeed);
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
}

SwordComponent::~SwordComponent() {}

//If the weapon is a sword it damages the player if its close enough
void SwordComponent::Attack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		if (IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
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
		if (IsInDistance(_owner->GetCurrentTarget()->GetPosition(), _owner->GetPosition(), _attackRange)) {
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
