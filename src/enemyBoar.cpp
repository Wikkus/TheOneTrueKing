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

EnemyBoar::EnemyBoar(unsigned int objectID, EnemyType enemyType) : EnemyBase(objectID, enemyType) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_boarSprite);

	_position = Vector2<float>(-10000.f, -10000.f);

	_circleCollider = std::make_shared<Circle>();
	_circleCollider->Init(_position, 16.f);

	_behaviorData.characterRadius = _circleCollider->GetRadius();

	_maxHealth = 150;
	_currentHealth = _maxHealth;

	_behaviorData.velocity = _velocity;

	_attackDamage = 20;
	_attackRange = 150;
	_attackRadius = std::make_shared<Circle>();
	_attackRadius->Init(_position, 150.f);

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 2.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 100.f;
	
	_behaviorData.linearTargetRadius = _attackRange - 5.f;
	_behaviorData.linearSlowDownRadius = _attackRange + 25.f;

	_behaviorData.separationThreshold = _circleCollider->GetRadius() * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
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

	_behaviorData.velocity = _velocity;	
	
	_isAttacking = false;
	_damagedPlayer = false;
	_playerInRange = false;
	_chargeAttackTimer->DeactivateTimer();
	_attackCooldownTimer->DeactivateTimer();
}

void EnemyBoar::Update() {
	_behaviorData.queriedObjects = objectBaseQuadTree->Query(_circleCollider);
	if (IsInDistance(_position, playerCharacter->GetPosition(), _attackRadius->GetRadius())) {
		_collidedWithPlayer = true;
	}
	if(!_isAttacking) {
		SetTargetPosition(playerCharacter->GetPosition());
		UpdateMovement();
	}
	HandleAttack();
	_circleCollider->SetPosition(_position);
	_attackRadius->SetPosition(_position);
	_collidedWithPlayer = false;
}

void EnemyBoar::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

bool EnemyBoar::HandleAttack() {
	if (_attackCooldownTimer->GetTimerActive()) {
		return true;
	}
	if (!_playerInRange) {
		if (IsInDistance(_position, playerCharacter->GetPosition(), _attackRadius->GetRadius())) {
			_playerInRange = true;
		}
	}
	if (_isAttacking) {
		_position += _dashDirection * _dashSpeed * deltaTime;
		if (!_damagedPlayer && _collidedWithPlayer) {
			playerCharacter->TakeDamage(_attackDamage);
			_damagedPlayer = true;
		}
		if ((_dashStartPosition - _position).absolute() > _dashDistance) {
			_attackCooldownTimer->ResetTimer();
			_isAttacking = false;
			_damagedPlayer = false;
			_playerInRange = false;
		}
	} else if (_chargeAttackTimer->GetTimerFinished()) {
		_isAttacking = true;
		_chargeAttackTimer->DeactivateTimer();
		_dashDirection = (playerCharacter->GetPosition() - _position).normalized();
		_dashStartPosition = _position;

	} else if (_playerInRange && !_chargeAttackTimer->GetTimerActive() && !_isAttacking) {
		_dashDistance = (playerCharacter->GetPosition() - _position).absolute() * 1.75f;
		_chargeAttackTimer->ResetTimer();
		_velocity = { 0.f, 0.f };
	}
	return true;
}

bool EnemyBoar::UpdateMovement() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();

	_position += _velocity * deltaTime;
	_orientation += _rotation * deltaTime;

	_circleCollider->SetPosition(_position);
	_behaviorData.rotation = _rotation;
	_behaviorData.velocity = _velocity;

	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
	_rotation += _steeringOutput.angularVelocity * deltaTime;
	_velocity += _steeringOutput.linearVelocity * deltaTime;

	if (_steeringOutput.linearVelocity.absolute() < FLT_EPSILON) {
		_velocity = { 0.f, 0.f };
	}
	_velocity = LimitVelocity(_velocity, _behaviorData.maxSpeed);
	return true;
}

