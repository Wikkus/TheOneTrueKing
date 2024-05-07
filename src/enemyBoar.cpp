#include "enemyBoar.h"

#include "dataStructuresAndMethods.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "steeringBehavior.h"
#include "timerManager.h"

#include <memory>

EnemyBoar::EnemyBoar(unsigned int objectID, EnemyType enemyType) :
	EnemyBase(objectID, enemyType) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_boarSprite);

	_position = Vector2<float>(-10000.f, -10000.f);

	_circleCollider.radius = 16.f;
	_circleCollider.position = _position;

	_maxHealth = 20;
	_currentHealth = _maxHealth;

	_attackDamage = 10;
	_attackRange = 100;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 2.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 100.f;
	
	_behaviorData.linearTargetRadius = _attackRange - 5.f;
	_behaviorData.linearSlowDownRadius = _attackRange + 25.f;

	_behaviorData.separationThreshold = _circleCollider.radius * 1.5f;
	_behaviorData.decayCoefficient = 1.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->ClearBehaviours();
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);

	_blendSteering->ClearBehaviours();
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);

	_attackCooldownTimer = timerManager->CreateTimer(1.f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);
	_chargeAttackTimer->DeactivateTimer();
}

EnemyBoar::~EnemyBoar() {}

void EnemyBoar::Init() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();
	_direction = _behaviorData.targetPosition - _position;
}

void EnemyBoar::Update() {
	_queriedObjects = objectBaseQuadTree->Query(_circleCollider);
	if(!_isAttacking) {
		SetTargetPosition(playerCharacter->GetPosition());
		UpdateMovement();
	}
	HandleAttack();
	_circleCollider.position = _position;
}

void EnemyBoar::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

void EnemyBoar::RenderText() {}

const Circle EnemyBoar::GetCollider() const {
	return _circleCollider;
}

const EnemyType EnemyBoar::GetEnemyType() const {
	return _enemyType;
}

const ObjectType EnemyBoar::GetObjectType() const {
	return _objectType;
}

const BehaviorData EnemyBoar::GetBehaviorData() const {
	return _behaviorData;
}

const float EnemyBoar::GetOrientation() const {
	return _orientation;
}

const float EnemyBoar::GetRotation() const {
	return _rotation;
}

const int EnemyBoar::GetCurrentHealth() const {
	return _currentHealth;
}

const unsigned int EnemyBoar::GetObjectID() const {
	return _objectID;
}

const std::shared_ptr<Sprite> EnemyBoar::GetSprite() const {
	return _sprite;
}

const Vector2<float> EnemyBoar::GetPosition() const {
	return _position;
}

const Vector2<float> EnemyBoar::GetVelocity() const {
	return _velocity;
}

const std::vector<std::shared_ptr<ObjectBase>> EnemyBoar::GetQueriedObjects() const {
	return _queriedObjects;
}

const std::shared_ptr<WeaponComponent> EnemyBoar::GetWeaponComponent() const {
	return _weaponComponent;
}

void EnemyBoar::ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position) {
	_orientation = orienation;
	_direction = direction;
	_position = position;
	_circleCollider.position = _position;
	Init();
}

void EnemyBoar::DeactivateEnemy() {
	_orientation = 0.f;
	_direction = Vector2<float>(0.f, 0.f);
	_position = Vector2<float>(-10000.f, -10000.f);
	_circleCollider.position = _position;
}

bool EnemyBoar::TakeDamage(unsigned int damageAmount) {
	_currentHealth -= damageAmount;
	if (_currentHealth <= 0) {
		return true;
	}
	return false;
}

void EnemyBoar::HandleAttack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return;
	}
	if (_isAttacking) {
		_position += _dashDirection * _dashSpeed * deltaTime;
		if (!_damagedPlayer) {	
			if (CircleIntersect(_circleCollider, playerCharacter->GetCircleCollider())) {
				playerCharacter->TakeDamage(_attackDamage);
				_damagedPlayer = true;
			}
		}
		if ((_dashStartPosition - _position).absolute() > _dashDistance) {
			_attackCooldownTimer->ResetTimer();
			_isAttacking = false;
			_damagedPlayer = false;
		}
	} else if (_chargeAttackTimer->GetTimerFinished()) {
		_isAttacking = true;
		_chargeAttackTimer->DeactivateTimer();
		_dashDirection = (playerCharacter->GetPosition() - _position).normalized();
		_dashStartPosition = _position;

	} else if (IsInDistance(playerCharacter->GetPosition(), _position, _attackRange) && !_chargeAttackTimer->GetTimerActive() && !_isAttacking) {
		_dashDistance = (playerCharacter->GetPosition() - _position).absolute() * 2.f;
		_chargeAttackTimer->ResetTimer();
	}
}

void EnemyBoar::SetPosition(Vector2<float> position) {
	_position = position;
}

void EnemyBoar::SetTargetPosition(Vector2<float> targetPosition) {
	_behaviorData.targetPosition = targetPosition;
}

void EnemyBoar::SetTargetOrientation(float targetOrientation) {	
	_behaviorData.targetOrientation = targetOrientation;
}

void EnemyBoar::SetVelocity(Vector2<float> velocity) {
	_velocity = velocity;
}

void EnemyBoar::UpdateMovement() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();

	_position += _velocity * deltaTime;
	_orientation += _rotation * deltaTime;

	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
	_rotation += _steeringOutput.angularVelocity * deltaTime;
	_velocity += _steeringOutput.linearVelocity * deltaTime;

	if (_velocity.absolute() > _behaviorData.maxSpeed) {
		_velocity.normalize();
		_velocity *= _behaviorData.maxSpeed;
	}
}

