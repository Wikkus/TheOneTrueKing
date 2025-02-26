#include "enemyBase.h"
#include "playerCharacter.h"
#include "weaponManager.h"

EnemyBase::EnemyBase(const EnemyType& enemyType) : ObjectBase(ObjectType::Enemy), _enemyType(enemyType) {
    _sprite = std::make_shared<Sprite>();
    _position = deactivatedPosition;

    _maxHealth = 100.f;
    _currentHealth = _maxHealth;

    _prioritySteering = std::make_shared<PrioritySteering>();
    _blendSteering = std::make_shared<BlendSteering>();

    _currentTarget = playerCharacters.back();
    _collider = std::make_shared<Circle>();
}

void EnemyBase::Render() {
    _sprite->RenderWithOrientation(0, _position, _orientation);
}

const std::shared_ptr<Collider> EnemyBase::GetCollider() const {
    return _collider;
}

const int EnemyBase::GetDamage() const {
    return _weaponComponent->GetAttackDamage();
}

const EnemyType EnemyBase::GetEnemyType() const {
    return _enemyType;
}

void EnemyBase::TakeDamage(const int& damageAmount) {
    _currentHealth -= damageAmount;
    if (_currentHealth <= 0) {
        enemyManager->RemoveObject(_objectID);
    }
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

const std::shared_ptr<WeaponComponent> EnemyBase::GetWeaponComponent() const {
    return _weaponComponent;
}

void EnemyBase::UpdateMovement() {
    UpdateAngularMovement();
    UpdateLinearMovement();
}

void EnemyBase::UpdateAngularMovement() {
    _orientation += _rotation * deltaTime;
    _rotation += _steeringOutput.angularVelocity * deltaTime;
}

void EnemyBase::UpdateLinearMovement() {
    _position += _velocity * deltaTime;
    _collider->SetPosition(_position);
    _velocity += _steeringOutput.linearVelocity * deltaTime;

    if (_steeringOutput.linearVelocity.absolute() < FLT_EPSILON) {
        _velocity = { 0.f, 0.f };
    }
    universalFunctions->LimitVelocity(_velocity, _behaviorData.maxSpeed);
}

void EnemyBase::ActivateEnemy(const float& orienation, const Vector2<float>& direction,
    const Vector2<float>& position, const WeaponType& weaponType) {
    _orientation = orienation;
    _direction = direction;
    _position = position;
    _collider->SetPosition(position);

    if (weaponType != WeaponType::Count) {
        _weaponComponent = weaponManager->SpawnWeapon(weaponType);
        _weaponComponent->SetOwner(shared_from_this());
        _weaponComponent->ActivateObject(position, direction, orienation);
        _weaponComponent->Init();
    }
    Init();
}

void EnemyBase::DeactivateObject() {
    _position = deactivatedPosition;
    _collider->SetPosition(_position);
    _formationIndex = -1;
    weaponManager->RemoveObject(_weaponComponent->GetObjectID());
    _weaponComponent = nullptr;
}

void EnemyBase::SetFormationIndex(const int& formationIndex) {
    _formationIndex = formationIndex;
}

void EnemyBase::SetPosition(const Vector2<float>& position) {
    _position = position;
    _collider->SetPosition(position);
}

void EnemyBase::SetTargetOrientation(const float& targetOrientation) {
    _behaviorData.targetOrientation = targetOrientation;
}

bool EnemyBase::ReplaceSteeringBehavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior) {
    return _prioritySteering->ReplaceSteeringBheavior(oldBehaviorType, newBehavior);
}