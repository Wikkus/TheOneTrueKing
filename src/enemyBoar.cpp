#include "enemyBoar.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "steeringBehavior.h"
#include "timerManager.h"

#include <memory>

EnemyBoar::EnemyBoar() : EnemyBase(EnemyType::Boar) {
	_sprite->Load(_boarSprite);

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->GetHeight() * 0.5f);

	_behaviorData.characterRadius = _sprite->GetHeight() * 0.5f;

	_attackDamage = 20;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 2.5f;
	_behaviorData.maxRotation = PI * 2.5f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 100.f;
	
	_behaviorData.separationThreshold = _behaviorData.characterRadius * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);
	_blendSteering->ClearBehaviours();

	_attackCooldownTimer = timerManager->CreateTimer(1.f);
	_chargeAttackTimer = timerManager->CreateTimer(0.5f);
	_chargeAttackTimer->DeactivateTimer();
}

EnemyBoar::~EnemyBoar() {}

void EnemyBoar::Init() {
	_currentTarget = playerCharacters.back();
	_targetPosition = _currentTarget->GetPosition();
	_direction = _targetPosition - _position;
	_velocity = { 0.f, 0.f };
	_rotation = 0.f;

	_behaviorData.linearTargetRadius = _weaponComponent->GetAttackRange() - 5.f;
	_behaviorData.linearSlowDownRadius = _weaponComponent->GetAttackRange() + 50.f;

	_currentHealth = _maxHealth + _weaponComponent->GetHealthModifier();

	_chargeAttackTimer->DeactivateTimer();
	_attackCooldownTimer->DeactivateTimer();
}

void EnemyBoar::Update() {
	if(!_weaponComponent->GetIsAttacking()) {
		_targetPosition = _currentTarget->GetPosition();
		_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
		UpdateMovement();
	}
	_weaponComponent->Update();
	_weaponComponent->Attack();
	_collider->SetPosition(_position);
}