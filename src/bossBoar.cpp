#include "bossBoar.h"
#include "decisionTree.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "timerManager.h"

#include <string>

BoarBoss::BoarBoss(unsigned int objectID, EnemyType enemyType) : EnemyBase(objectID, enemyType) {
	_sprite->Load(_bossBoarSpritePath);

	_position = Vector2<float>(-10000.f, -10000.f);
	_circleCollider->Init(_position, _sprite->h * 0.5f);

	_behaviorData.characterRadius = _circleCollider->GetRadius();

	_maxHealth = 1500;
	_currentHealth = _maxHealth;

	_attackDamage = 300;
	_attackRange = 300;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 1.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 50.f;
	_behaviorData.maxSpeed = 100.f;

	_behaviorData.linearTargetRadius = 5.f;
	_behaviorData.linearSlowDownRadius = _attackRange + 25.f;

	_behaviorData.separationThreshold = _circleCollider->GetRadius() * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);

	_attackCooldownTimer = timerManager->CreateTimer(2.f);
	_chargeAttackTimer = timerManager->CreateTimer(2.5f);
	_attackCooldownTimer->DeactivateTimer();
	_chargeAttackTimer->DeactivateTimer();
	_healthTextSprite = std::make_shared<TextSprite>();

	CreateDecisionTree();
}

BoarBoss::~BoarBoss() {}

void BoarBoss::Init() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();
	_position = Vector2<float>(windowWidth * 0.9f, windowHeight * 0.3f);
	_direction = _behaviorData.targetPosition - _position;
	_behaviorData.velocity = _velocity;

	_healthTextSprite->Init("res/roboto.ttf", 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_healthTextSprite->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.9f));

	MakeDecision();
}

void BoarBoss::Update() {
	if (_decisionTreeAction) {
		_decisionTreeAction->ExecuteAction(*this);
		if (_decisionTreeAction->GetActionFinished()) {
			MakeDecision();
		}
	}
}

void BoarBoss::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

void BoarBoss::RenderText() {
	_healthTextSprite->Render();
}

void BoarBoss::HandleAttack() {
	if (!_isAttacking && !_attackCooldownTimer->GetTimerActive()) {
		_isAttacking = true;
		_behaviorData.targetPosition = playerCharacter->GetPosition();
		_dashDistance = (_behaviorData.targetPosition - _position).absolute() + 200.f;
		_velocity = { 0.f, 0.f };
		_chargeAttackTimer->ResetTimer();
	}
	if (_chargeAttackTimer->GetTimerActive() || _attackCooldownTimer->GetTimerActive()) {
		_behaviorData.targetPosition = playerCharacter->GetPosition();

		_orientation += _rotation * deltaTime;
		_behaviorData.rotation = _rotation;

		_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
		_rotation += _steeringOutput.angularVelocity * deltaTime;

	} else if (_chargeAttackTimer->GetTimerFinished()) {
		_chargeAttackTimer->DeactivateTimer();
		_dashDirection = (_behaviorData.targetPosition - _position).normalized();
		_dashStartPosition = _position;

	} else {
		if (!_damagedPlayer && IsInDistance(_position, playerCharacter->GetPosition(), _circleCollider->GetRadius())) {
			playerCharacter->TakeDamage(_attackDamage);
			_damagedPlayer = true;
		}
		if (_isAttacking) {
			_position += _dashDirection * _dashSpeed * deltaTime;
			_circleCollider->SetPosition(_position);
			if ((_dashStartPosition - _position).absolute() > _dashDistance) {
				_attackCooldownTimer->ResetTimer();
				_isAttacking = false;
				_damagedPlayer = false;
			}
		}
		if (_attackCooldownTimer->GetTimerFinished()) {
			_attackCooldownTimer->DeactivateTimer();
			_decisionTreeAction->SetActionFinished(true);
		}
	}
}

void BoarBoss::UpdateMovement() {
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
}

void BoarBoss::CreateDecisionTree() {
	_decisionTree = std::make_shared<WithinRangeDecision>(_attackRange, 0);
		std::shared_ptr<MoveAction> moveAction = std::make_shared<MoveAction>();
		_decisionTree->SetFalseNode(moveAction);
		std::shared_ptr<AttackAction> attackAction = std::make_shared<AttackAction>();
		_decisionTree->SetTrueNode(attackAction);
	
}
void BoarBoss::MakeDecision() {
	_decisionMade = _decisionTree->MakeDecision(*this);
	if (_decisionMade->GetNodeType() == NodeType::ActionNode) {
		_decisionTreeAction = std::static_pointer_cast<Action>(_decisionMade);
		_decisionTreeAction->SetActionFinished(false);
	}
	//Node is not an action, recursion failed

}

void BoarBoss::FireProjectile() {
	_projectileDirection = (_behaviorData.targetPosition - _position).normalized();
	projectileManager->SpawnProjectile(ProjectileType::Energyblast, VectorAsOrientation(_projectileDirection), _projectileDirection, _position, 500, 100);
}