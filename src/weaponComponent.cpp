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
	
	_isAttacking = false;
}

void WeaponComponent::Init() {
	_isAttacking = false;
}

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

void WeaponComponent::SetOwner(std::shared_ptr<ObjectBase> owner) {
	_owner = owner;
}

void WeaponComponent::SetWeaponValues(std::shared_ptr<ObjectBase> owner, const bool& renderWeapon, const int& attackDamage, const float& attackRange, const float& attackCooldown, const float& chargeAttackTime) {
	_owner = owner;	
	_renderWeapon = renderWeapon;

	_attackDamage = attackDamage;
	_attackRange = attackRange;
	_attackCooldownTimer->SetTimeInSeconds(attackCooldown);
	_chargeAttackTimer->SetTimeInSeconds(chargeAttackTime);
}

void WeaponComponent::DeactivateObject() {
	_position = deactivatedPosition;
	_collider->SetPosition(_position);
	SetValuesToDefault();
	DeactivateTimers();
	_owner = nullptr;
}

ShieldComponent::ShieldComponent() {
	_sprite->Load(_path);

	_attackCooldownTimer = timerHandler->SpawnTimer(1.f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(0.5f, false, false);
	SetValuesToDefault();

	_weaponType = WeaponType::Shield;
}

ShieldComponent::~ShieldComponent() {}

bool ShieldComponent::UpdateAttack() {
	ExecuteAttack();
	return true;
}

bool ShieldComponent::ExecuteAttack() {
	return true;
}

void ShieldComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(1.f);
	_chargeAttackTimer->SetTimeInSeconds(0.5f);
	_attackRange = 25.f;
	_attackDamage = 10;
	_healthModifier = 125;
	_renderWeapon = true;
}

StaffComponent::StaffComponent() {
	_sprite->Load(_path);
	_attackCooldownTimer = timerHandler->SpawnTimer(1.f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(0.5f, false, false);

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
	_isAttacking = false;
}

//If the weapon is a staff it shoots a fireball towards the player
bool StaffComponent::UpdateAttack() {
	if (_attackCooldownTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_chargeAttackTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_isAttacking && _chargeAttackTimer->GetIsFinished()) {
		ExecuteAttack();
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();
		return true;

	} else if (!_isAttacking) {	
		if (_unlimitedRange) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;

		} else if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->ResetTimer();
			_isAttacking = true;
		}
	}
	return false;
}

bool StaffComponent::ExecuteAttack() {
	_direction = ((_owner->GetTargetObject()->GetPosition() - _owner->GetPosition()).normalized());
	projectileManager->SpawnProjectile(_owner, _projectileType, universalFunctions->VectorAsOrientation(_direction),
		_direction, _position, _attackDamage, _projectileSpeed);
	_owner->SetVelocity({ 0.f, 0.f });
	return true;
}

void StaffComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(1.f);
	_chargeAttackTimer->SetTimeInSeconds(1.f);
	_attackRange = 300.f;
	_attackDamage = 10;
	_healthModifier = -25;
	_projectileType = ProjectileType::EnemyFireball;
	_projectileSpeed = 200.f;
	_renderWeapon = true;
}

void StaffComponent::SetProjectileValues(const ProjectileType& projectileType, const bool& unlimitedRange, const float& projectileSpeed) {
	_projectileType = projectileType;
	_unlimitedRange = unlimitedRange;
	_projectileSpeed = projectileSpeed;
}

SuperStaffComponent::SuperStaffComponent() {
	_sprite->Load(_path);

	_attackCooldownTimer->SetTimeInSeconds(0.25f);

	_attackDamage = 150;
	_healthModifier = 0;

	_weaponType = WeaponType::SuperStaff;
}

SuperStaffComponent::~SuperStaffComponent() {
}

bool SuperStaffComponent::UpdateAttack() {
	if (_attackCooldownTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	ExecuteAttack();
	_attackCooldownTimer->ResetTimer();
	return true;
}

bool SuperStaffComponent::ExecuteAttack() {
	_multiShotAngle = (-_angleOffset * _multiShotAmount);
	_multiShotAngle /= _multiShotAmount;

	_direction = (_owner->GetTargetObject()->GetPosition() - _owner->GetPosition()).normalized();
	for (unsigned int i = 0; i < _multiShotAmount; i++) {
		_multiShotDirection = _direction.rotated(_multiShotAngle);

		projectileManager->SpawnProjectile(_owner, _projectileType, universalFunctions->VectorAsOrientation(_direction) + _multiShotAngle,
			_multiShotDirection, _position, _attackDamage, _projectileSpeed);
		_multiShotAngle += _angleOffset;
	}
	return true;
}

void SuperStaffComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(0.25f);
	_attackDamage = 150;
	_healthModifier = 0;
	_projectileSpeed = 200.f;
	_renderWeapon = true;
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

//If the weapon is a sword it damages the player if its close enough
bool SwordComponent::UpdateAttack() {
	if (_attackCooldownTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_chargeAttackTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_isAttacking && _chargeAttackTimer->GetIsFinished()) {
		ExecuteAttack();
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();
		return true;

	} else if (!_isAttacking && universalFunctions->IsInDistance(_owner->GetPosition(), playerCharacters.back()->GetPosition(), _attackRange)) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
	return false;
}

bool SwordComponent::ExecuteAttack() {
	if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
		_owner->GetTargetObject()->TakeDamage(_attackDamage);
	}
	_owner->SetVelocity({ 0.f, 0.f });
	_owner->SetRotation(0.f);
	return true;
}

void SwordComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(0.75f);
	_chargeAttackTimer->SetTimeInSeconds(0.25f);
	_attackRange = 25.f;
	_attackDamage = 15;
	_healthModifier = 25;
	_renderWeapon = true;
}

WarstompComponent::WarstompComponent() {
	_sprite->Load(_path);
	_attackCooldownTimer = timerHandler->SpawnTimer(2.f, false, false);
	_chargeAttackTimer = timerHandler->SpawnTimer(1.f, false, false);

	_attackRange = 200.f;

	_attackDamage = 300;
	_healthModifier = 0;

	_weaponType = WeaponType::Warstomp;
}

//If weaponComponent is a warstomp, it will damage the player if it is in range of the AOE
bool WarstompComponent::UpdateAttack() {
	if (_attackCooldownTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_chargeAttackTimer->GetIsActive()) {
		debugDrawer->AddDebugCircle(_owner->GetPosition(), _attackRange, { 255, 0, 0, 255 });
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_isAttacking && _chargeAttackTimer->GetIsFinished()) {
		ExecuteAttack();
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();
		return true;		

	} else if (!_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
	return false;
}

bool WarstompComponent::ExecuteAttack() {
	if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
		_owner->GetTargetObject()->TakeDamage(_attackDamage);
	}
	_owner->SetVelocity({ 0.f, 0.f });
	_owner->SetRotation(0.f);
	return true;
}

void WarstompComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(2.f);
	_chargeAttackTimer->SetTimeInSeconds(2.f);
	_attackRange = 200.f;
	_attackDamage = 300;
	_healthModifier = 0;
	_renderWeapon = true;
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

bool TusksComponent::UpdateAttack() {
	_oldPosition = _owner->GetPosition();
	if (_attackCooldownTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;

	} else if (_chargeAttackTimer->GetIsActive()) {
		if (_owner->GetObjectType() == ObjectType::Enemy) {
			enemyManager->CastAsEnemy(_owner)->UpdateAngularMovement();
		}
		return false;
	}
	if (_isAttacking) {
		if (ExecuteAttack()) {
			_attackCooldownTimer->ResetTimer();
			_chargeAttackTimer->SetTimer(false, false);
			_isAttacking = false;
			_owner->SetVelocity({ 0.f, 0.f });
			_owner->SetRotation(0.f);
			return true;
		}
	} else {
		if(_chargeAttackTimer->GetIsFinished()) {
			if (_isJumpback) {
				_dashDirection = (_owner->GetPosition() - _owner->GetTargetObject()->GetPosition());
				_dashDistance = _jumpBackDistance;

			} else {
				_dashDirection = (_owner->GetTargetObject()->GetPosition() - _owner->GetPosition());
				_dashDistance = _dashDirection.absolute() + 100.f;
			}
			_dashDirection.normalize();
			_dashStartPosition = _owner->GetPosition();
			_distanceTraveled = 0.f;
			_isAttacking = true;	
		
		} else if (universalFunctions->IsInDistance(_owner->GetTargetObject()->GetPosition(), _owner->GetPosition(), _attackRange)) {
			_chargeAttackTimer->ResetTimer();
			_owner->SetVelocity({ 0.f, 0.f });
			_owner->SetRotation(0.f);
		}
	}
	return false;
}

bool TusksComponent::ExecuteAttack() {
	_owner->SetPosition(_owner->GetPosition() + _dashDirection * _dashSpeed * deltaTime);
	_distanceTraveled += _dashSpeed * deltaTime;
	_formerPosition = _owner->GetPosition();
	if (universalFunctions->OutsideBorderX(_owner->GetPosition().x, _owner->GetSprite()->GetHeight() * 0.25f)) {
		_formerPosition.x = _oldPosition.x;
		_owner->SetPosition(_formerPosition);
	}
	if (universalFunctions->OutsideBorderY(_owner->GetPosition().y, _owner->GetSprite()->GetHeight() * 0.25f)) {
		_formerPosition.y = _oldPosition.y;
		_owner->SetPosition(_formerPosition);
	}
	if (!_isJumpback) {
		if (!_damageCooldown->GetIsActive()) {
			for (unsigned int i = 0; i < _owner->GetQueriedObjects().size(); i++) {
				if (_owner->GetQueriedObjects()[i]->GetObjectType() == _owner->GetTargetObject()->GetObjectType()) {
					_owner->GetQueriedObjects()[i]->TakeDamage(_attackDamage);
					_damageCooldown->ResetTimer();
				}
			}
		}
	}
	if (_distanceTraveled > _dashDistance) {
		return true;

	} 

	return false;
}

void TusksComponent::SetValuesToDefault() {
	_attackCooldownTimer->SetTimeInSeconds(1.5f);
	_chargeAttackTimer->SetTimeInSeconds(1.f);
	_damageCooldown->SetTimeInSeconds(0.5f);
	_attackDamage = 20.f;
	_attackRange = 200.f;
	_healthModifier = 75;
	_isJumpback = false;
	_renderWeapon = true;
}

void TusksComponent::SetIsJumpback(bool isJumpback) {
	_isJumpback = isJumpback;
}
