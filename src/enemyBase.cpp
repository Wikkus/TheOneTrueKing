#include "enemyBase.h"
#include "playerCharacter.h"

bool EnemyBase::TakeDamage(unsigned int damageAmount) {
    _currentHealth -= damageAmount;
    if (_currentHealth <= 0) {
        return true;
    }
    return false;
}

EnemyBase::EnemyBase(int objectID, EnemyType enemyType) :
    ObjectBase(objectID, ObjectType::Enemy), _enemyType(enemyType) {
    _sprite = std::make_shared<Sprite>();
    _position = Vector2<float>(-10000.f, -10000.f);

    _circleCollider = std::make_shared<Circle>();

    _prioritySteering = std::make_shared<PrioritySteering>();
    _blendSteering = std::make_shared<BlendSteering>();
}

const std::shared_ptr<Collider> EnemyBase::GetCollider() const {
    return _circleCollider;
}

const float EnemyBase::GetAttackRange() const {
    return _attackRange;
}

const EnemyType EnemyBase::GetEnemyType() const {
    return _enemyType;
}

const float EnemyBase::GetRotation() const {
    return _rotation;
}


const int EnemyBase::GetCurrentHealth() const {
    return _currentHealth;
}

const int EnemyBase::GetFormationIndex() const {
    return _formationIndex;
}

const BehaviorData EnemyBase::GetBehaviorData() const {
    return _behaviorData;
}

const std::vector<std::shared_ptr<ObjectBase>> EnemyBase::GetQueriedObjects() const {
    return _behaviorData.queriedObjects;
}

const std::shared_ptr<WeaponComponent> EnemyBase::GetWeaponComponent() const {
    return _weaponComponent;
}

void EnemyBase::HandleAttack() {}

void EnemyBase::UpdateMovement() {}

void EnemyBase::ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) {
    _orientation = orienation;
    _direction = direction;
    _position = position;
    _circleCollider->SetPosition(position);
    if (weaponType != WeaponType::Count) {
        _weaponComponent = enemyManager->AccessWeapon(weaponType);
    }
    Init();
}

void EnemyBase::DeactivateEnemy() {
    _orientation = 0.f;
    _direction = { 0.f, 0.f };
    _position = { -10000.f, 10000.f };
    _behaviorData.rotation = 0.f;
    _circleCollider->SetPosition(_position);
    _velocity = { 0.f, 0.f };
    _behaviorData.velocity = _velocity;
    _formationIndex = -1;
    if (_weaponComponent) {
        _weaponComponent->DeactivateTimers();
    }
}

void EnemyBase::SetFormationIndex(int formationIndex) {
    _formationIndex = formationIndex;
}

void EnemyBase::SetTargetPosition(Vector2<float> targetPosition) {
    _behaviorData.targetPosition = targetPosition;
}

void EnemyBase::SetTargetOrientation(float targetOrientation) {
    _behaviorData.targetOrientation = targetOrientation;
}

bool EnemyBase::ReplaceSteeringBehavior(SteeringBehaviorType oldBehaviorType, BehaviorAndWeight newBehavior) {
    return _prioritySteering->ReplaceSteeringBheavior(oldBehaviorType, newBehavior);
}