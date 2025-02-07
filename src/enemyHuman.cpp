#include "enemyHuman.h"

#include "dataStructuresAndMethods.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "quadTree.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "timerManager.h"
#include "weaponComponent.h"

#include <memory>

EnemyHuman::EnemyHuman(unsigned int objectID, EnemyType enemyType) :
	EnemyBase(objectID, enemyType) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_humanSprite);
	_position = Vector2<float>(-10000.f, -10000.f);

	_circleCollider = std::make_shared<Circle>();
	_circleCollider->Init(_position, 16.f);
	_behaviorData.characterRadius = _circleCollider->GetRadius();

	_numberOfWeaponTypes = (int)(WeaponType::Count);
	_maxHealth = 75;
	_currentHealth = _maxHealth;

	_behaviorData.velocity = _velocity;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = FLT_EPSILON;
	_behaviorData.maxAngularAcceleration = PI * 2.f;
	_behaviorData.maxRotation = PI * 2.f;
	
	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 75.f;

	_behaviorData.separationThreshold = _circleCollider->GetRadius() * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();
	
	_alignBehavior.steeringBehaviour = std::make_shared<AlignBehavior>();
	_alignBehavior.weight = 1.f;

	_faceBehavior.steeringBehaviour = std::make_shared<FaceBehavior>();
	_faceBehavior.weight = 1.f;
}

EnemyHuman::~EnemyHuman() {}

void EnemyHuman::Init() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();
	_direction = Vector2<float>(_behaviorData.targetPosition - _position).normalized();
	_orientation = VectorAsOrientation(_direction);
	_behaviorData.velocity = _velocity;

	_prioritySteering->ClearGroups();
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
	_behaviorData.linearTargetRadius = 5.f;
	_behaviorData.linearSlowDownRadius = 20.f;

	if (gameStateHandler->GetGameMode() == GameMode::Formation) {
		_blendSteering->AddSteeringBehaviour(_alignBehavior);

	} else {
		if (_weaponComponent->GetWeaponType() == WeaponType::Staff) {
			std::uniform_real_distribution dist{ _weaponComponent->GetAttackRange() * 0.5f, _weaponComponent->GetAttackRange() };
			_linearTargetRadius = dist(randomEngine);
			_behaviorData.linearTargetRadius = _linearTargetRadius;
			_behaviorData.linearSlowDownRadius = _linearTargetRadius * 1.5f;
		}
		_blendSteering->AddSteeringBehaviour(_faceBehavior);
	}
	_prioritySteering->AddGroup(*_blendSteering);
}

void EnemyHuman::Update() {
	_behaviorData.queriedObjects = objectBaseQuadTree->Query(_circleCollider);
	UpdateTarget();
	HandleAttack();

	UpdateMovement();
}

void EnemyHuman::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
	_weaponComponent->Render(_position, _orientation);
}

void EnemyHuman::RenderText() {}

const std::shared_ptr<Collider> EnemyHuman::GetCollider() const {
	return _circleCollider;
}

const EnemyType EnemyHuman::GetEnemyType() const {
	return _enemyType;
}

const ObjectType EnemyHuman::GetObjectType() const {
	return _objectType;
}

const float EnemyHuman::GetOrientation() const {
	return _orientation;
}

const float EnemyHuman::GetRotation() const {
	return _rotation;
}

const int EnemyHuman::GetCurrentHealth() const {
	return _currentHealth;
}

const unsigned int EnemyHuman::GetObjectID() const {
	return _objectID;
}

const int EnemyHuman::GetFormationIndex() const {
	return _formationIndex;
}

const std::shared_ptr<Sprite> EnemyHuman::GetSprite() const {
	return _sprite;
}

const BehaviorData EnemyHuman::GetBehaviorData() const {
	return _behaviorData;
}

const Vector2<float> EnemyHuman::GetPosition() const {
	return _position;
}

const Vector2<float> EnemyHuman::GetVelocity() const {
	return _velocity;
}

const std::vector<std::shared_ptr<ObjectBase>> EnemyHuman::GetQueriedObjects() const {
	return _behaviorData.queriedObjects;
}

const std::shared_ptr<WeaponComponent> EnemyHuman::GetWeaponComponent() const {
	return _weaponComponent;
}

void EnemyHuman::ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) {
	_orientation = orienation;
	_direction = direction;
	_position = position;
	_circleCollider->SetPosition(position);
	PickWeapon(weaponType);
	Init();
}

void EnemyHuman::DeactivateEnemy() {
	_orientation = 0.f;
	_direction = Vector2<float>(0.f, 0.f);
	_position = Vector2<float>(-10000.f, -10000.f);
	_behaviorData.rotation = 0.f;
	_circleCollider->SetPosition(_position);
	_velocity = { 0.f, 0.f };
	_behaviorData.velocity = _velocity;
	_weaponComponent->DeactivateTimers();
	_formationIndex = -1;
}

bool EnemyHuman::HandleAttack() {
	//Depending on the weapon, the attack works differently
	_weaponComponent->Attack(_position, _orientation);
	return true;
}

void EnemyHuman::SetFormationIndex(int formationIndex) {
	_formationIndex = formationIndex;
}

void EnemyHuman::SetPosition(Vector2<float> position) {
	_position = position;
}

void EnemyHuman::SetTargetPosition(Vector2<float> targetPosition) {
	_behaviorData.targetPosition = targetPosition;
}

void EnemyHuman::SetTargetOrientation(float targetOrientation) {
	_behaviorData.targetOrientation = targetOrientation;
}

void EnemyHuman::SetVelocity(Vector2<float> velocity) {
	_velocity = velocity;
	_behaviorData.velocity = _velocity;
}

bool EnemyHuman::TakeDamage(unsigned int damageAmount) {
	_currentHealth -= damageAmount;
	if (_currentHealth <= 0) {	
		return true;
	}
	return false;
}

bool EnemyHuman::UpdateMovement() {
	_position += _velocity * deltaTime;
	_orientation += _rotation * deltaTime;

	_circleCollider->SetPosition(_position);
	_behaviorData.rotation = _rotation;
	_behaviorData.velocity = _velocity;

	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
	_rotation += _steeringOutput.angularVelocity * deltaTime;
	_velocity += _steeringOutput.linearVelocity * deltaTime;

	//When steering linear velocity is 0, then the target is reached and the character should stop.
	if (_steeringOutput.linearVelocity.absolute() < FLT_EPSILON) {
		_velocity = { 0.f, 0.f };
	}
	_velocity = LimitVelocity(_velocity, _behaviorData.maxSpeed);
	return true;
}

void EnemyHuman::UpdateTarget() {
	if (gameStateHandler->GetGameMode() == GameMode::Formation) {
		if (_weaponComponent->GetWeaponType() == WeaponType::Sword) {
			//When the formation is at the target position, the swordfighters will change target and run to the player
			switch (_currentTarget) {
			case CurrentTarget::Player:
				_behaviorData.targetPosition = playerCharacter->GetPosition();
				break;
			case CurrentTarget::SlotFormation:
				if (enemyManager->GetFormationManagers()[_formationIndex]->GetInPosition()) {
					if (!_prioritySteering->ReplaceSteeringBheavior(SteeringBehaviorType::Align, BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f), 0)) {
						_prioritySteering->AddBehaviorInGroup(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f), 0);
					}
					_currentTarget = CurrentTarget::Player;
				}
				break;
			case CurrentTarget::Count:
				break;
			default:
				break;
			}
		}
	} else {
		_behaviorData.targetPosition = playerCharacter->GetPosition();
	}
}

void EnemyHuman::PickWeapon(WeaponType weaponType) {
	_weaponComponent = enemyManager->AccessWeapon(weaponType);
	_maxHealth += _weaponComponent->GetHealthModifier();
	_currentHealth = _maxHealth;
}