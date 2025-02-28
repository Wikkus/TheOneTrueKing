#include "enemyHuman.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "quadTree.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "weaponComponent.h"
#include "weaponManager.h"

#include <memory>

EnemyHuman::EnemyHuman() : EnemyBase(EnemyType::Human) {
	_sprite->Load(_humanSprite);

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->GetHeight() * 0.5f);

	_behaviorData.characterRadius = _sprite->GetHeight() * 0.5f;

	_numberOfWeaponTypes = (int)(WeaponType::Count);

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = FLT_EPSILON;
	_behaviorData.maxAngularAcceleration = PI * 2.f;
	_behaviorData.maxRotation = PI * 2.f;
	
	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 75.f;

	_behaviorData.separationThreshold = _behaviorData.characterRadius * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_behaviorData.linearTargetRadius = 5.f;
	_behaviorData.linearSlowDownRadius = 20.f;
}

EnemyHuman::~EnemyHuman() {}

void EnemyHuman::Init() {
	_currentTarget = playerCharacters.back();
	_targetPosition = _currentTarget->GetPosition();
	_direction = Vector2<float>(_targetPosition - _position).normalized();
	_orientation = universalFunctions->VectorAsOrientation(_direction);
	_velocity = { 0.f, 0.f };
	
	_currentHealth = _maxHealth + _weaponComponent->GetHealthModifier();

	if (gameStateHandler->GetGameMode() == GameMode::Formation) {
		_behaviorData.linearTargetRadius = 5.f;
		_behaviorData.linearSlowDownRadius = 25.f;

	} else {
		std::uniform_real_distribution dist{ _weaponComponent->GetAttackRange() * 0.5f, _weaponComponent->GetAttackRange() };
		_linearTargetRadius = dist(randomEngine);
		_behaviorData.linearTargetRadius = _linearTargetRadius;
		_behaviorData.linearSlowDownRadius = _linearTargetRadius * 2.f;
	}
	switch (gameStateHandler->GetGameMode()) {
	case GameMode::Formation:				
		if (!_prioritySteering->ReplaceSteeringBehavior(SteeringBehaviorType::Face, _steeringBehviors[SteeringBehaviorType::Align])) {
			_prioritySteering->AddBehaviorInGroup(_steeringBehviors[SteeringBehaviorType::Align], 0);
		}
		break;
	case GameMode::Survival:
		if (!_prioritySteering->ReplaceSteeringBehavior(SteeringBehaviorType::Align, _steeringBehviors[SteeringBehaviorType::Face])) {
			_prioritySteering->AddBehaviorInGroup(_steeringBehviors[SteeringBehaviorType::Face], 0);
		}
		break;
	default:
		break;
	}
}

void EnemyHuman::Update() {
	UpdateTarget();
	//Depending on the weapon, the attack works differently
	_weaponComponent->HandleAttack();

	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
	UpdateMovement();
}

void EnemyHuman::UpdateTarget() {
	if (gameStateHandler->GetGameMode() == GameMode::Formation) {
		if (_weaponComponent->GetWeaponType() == WeaponType::Sword) {
			//When the formation is at the target position, the swordfighters will change target and run to the player
			switch (_currentTargetState) {
			case CurrentTarget::Player:
				_targetPosition = _currentTarget->GetPosition();
				break;
			case CurrentTarget::SlotFormation:
				if (enemyManager->GetFormationManagers()[_formationIndex]->GetInPosition()) {
					if (!_prioritySteering->ReplaceSteeringBehavior(SteeringBehaviorType::Align, _steeringBehviors[SteeringBehaviorType::Face])) {
						_prioritySteering->AddBehaviorInGroup(_steeringBehviors[SteeringBehaviorType::Face], 0);
					}
					_currentTargetState = CurrentTarget::Player;
				}
				break;
			case CurrentTarget::Count:
				break;
			default:
				break;
			}
		}
	} else {
		_targetPosition = _currentTarget->GetPosition();
	}
}