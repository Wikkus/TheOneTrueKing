#include "enemyBase.h"
#include "playerCharacter.h"
#include "weaponManager.h"
#include "stateStack.h"

EnemyBase::EnemyBase(const EnemyType& enemyType) : ObjectBase(ObjectType::Enemy), _enemyType(enemyType) {
    _sprite = std::make_shared<Sprite>();
    _position = deactivatedPosition;

    _maxHealth = 100.f;
    _currentHealth = _maxHealth;

    _currentTarget = playerCharacters.back();
    _collider = std::make_shared<Circle>();

    _prioritySteering = std::make_shared<PrioritySteering>();
    _blendSteering = std::make_shared<BlendSteering>();

    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Align, BehaviorAndWeight(std::make_shared<AlignBehavior>(), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Face, BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::LookAtDirection, BehaviorAndWeight(std::make_shared<LookAtDirectionBehavior>(), 1.f)));
    
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Separation, BehaviorAndWeight(std::make_shared<SeparationBehavior>(SteeringBehaviorType::Separation), 1.5f))); 
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Attraction, BehaviorAndWeight(std::make_shared<SeparationBehavior>(SteeringBehaviorType::Attraction), 1.5f)));

    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::CollisionAbvoidance, BehaviorAndWeight(std::make_shared<CollisionAvoidanceBehavior>(), 2.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::ObstacleAvoidance, BehaviorAndWeight(std::make_shared<ObstacleAvoidanceBehavior>(), 1.f)));

    
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Arrive, BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Seek, BehaviorAndWeight(std::make_shared<SeekBehavior>(SteeringBehaviorType::Seek), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Flee, BehaviorAndWeight(std::make_shared<SeekBehavior>(SteeringBehaviorType::Flee), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Pursue, BehaviorAndWeight(std::make_shared<PursueBehavior>(SteeringBehaviorType::Pursue), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Evade, BehaviorAndWeight(std::make_shared<PursueBehavior>(SteeringBehaviorType::Evade), 1.f)));
    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::VelocityMatch, BehaviorAndWeight(std::make_shared<VelocityMatchBehavior>(), 1.f)));

    _steeringBehviors.insert(std::make_pair(SteeringBehaviorType::Wander, BehaviorAndWeight(std::make_shared<WanderBehavior>(), 1.f)));

    SetDefaultBehaviors();
}

void EnemyBase::Render() {
    _sprite->RenderWithOrientation(0, _position, _orientation);
    _weaponComponent->Render();
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

void EnemyBase::SetWeaponComponent(std::shared_ptr<WeaponComponent> weaponComponent) {
    _weaponComponent = weaponComponent;
}

const std::shared_ptr<BlendSteering> EnemyBase::GetBlendSteering() const {
    return _blendSteering;
}

const std::shared_ptr<PrioritySteering> EnemyBase::GetPrioritySteering() const {
    return _prioritySteering;
}

std::unordered_map<SteeringBehaviorType, BehaviorAndWeight> EnemyBase::GetSteeringBehviors() const {
    return _steeringBehviors;
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
        _weaponComponent = weaponManager->SpawnWeapon(weaponType, shared_from_this());
        _weaponComponent->ActivateObject(position, direction, orienation);
        _weaponComponent->Init();
    }
    Init();
}

void EnemyBase::DeactivateObject() {
    _position = deactivatedPosition;
    _collider->SetPosition(_position);
    _formationIndex = -1;
    SetDefaultBehaviors();
    weaponManager->RemoveObject(_weaponComponent->GetObjectID());
    _weaponComponent = nullptr;
}

void EnemyBase::SetDefaultBehaviors() {
    _prioritySteering->ClearGroups();
    _blendSteering->AddSteeringBehavior(_steeringBehviors[SteeringBehaviorType::Arrive]);
    _blendSteering->AddSteeringBehavior(_steeringBehviors[SteeringBehaviorType::Face]);
    _blendSteering->AddSteeringBehavior(_steeringBehviors[SteeringBehaviorType::Separation]);
    _prioritySteering->AddGroup(*_blendSteering);
    _blendSteering->ClearBehaviors();
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