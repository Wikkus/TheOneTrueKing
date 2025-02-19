#include "bossBoar.h"
#include "debugDrawer.h"
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

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 1.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 50.f;
	_behaviorData.maxSpeed = 100.f;

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

	_weaponComponent = enemyManager->AccessWeapon(WeaponType::Tusks);
	_behaviorData.linearTargetRadius = 50.f;
	_behaviorData.linearSlowDownRadius = 200.f;

	CreateDecisionTree();
}

BoarBoss::~BoarBoss() {}

std::shared_ptr<Vector2<float>> temp = nullptr;

void BoarBoss::Init() {
	_targetPosition = _currentTarget->GetPosition();

	_position = Vector2<float>(windowWidth * 0.9f, windowHeight * 0.3f);
	_direction = _targetPosition - _position;

	_healthTextSprite->Init("res/roboto.ttf", 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_healthTextSprite->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.9f));

	MakeDecision();
}

void BoarBoss::Update() {
	_targetPosition = playerCharacters.back()->GetPosition();
	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);

	if (_decisionTreeAction) {
		if (_decisionTreeAction->ExecuteAction(*this)) {
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

void BoarBoss::TakeDamage(unsigned int damageAmount) {
	_currentHealth -= damageAmount;
	if (_currentHealth <= 0) {
		enemyManager->RemoveEnemy(_enemyType, _objectID);
	}
	_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
}

void BoarBoss::HandleAttack() {
	if (!_isAttacking && !_attackCooldownTimer->GetTimerActive()) {
		_isAttacking = true;
		_targetPosition = _currentTarget->GetPosition();
		_dashDistance = (_targetPosition - _position).absolute() + 200.f;
		_velocity = { 0.f, 0.f };
		_chargeAttackTimer->ResetTimer();
	}
	if (_chargeAttackTimer->GetTimerActive() || _attackCooldownTimer->GetTimerActive()) {
		_targetPosition = _currentTarget->GetPosition();

		_orientation += _rotation * deltaTime;
		_rotation = _rotation;

		_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
		_rotation += _steeringOutput.angularVelocity * deltaTime;

	} else if (_chargeAttackTimer->GetTimerFinished()) {
		_chargeAttackTimer->DeactivateTimer();
		_dashDirection = (_targetPosition - _position).normalized();
		_dashStartPosition = _position;

	} else {
		if (!_damagedPlayer && IsInDistance(_position, _currentTarget->GetPosition(), _circleCollider->GetRadius())) {
			_currentTarget->TakeDamage(_attackDamage);
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
		}
	}
}

void BoarBoss::CreateDecisionTree() {
	_decisionTree = std::make_shared<WithinRangeDecision>(75, 0);
		std::shared_ptr<MoveAction> moveAction = std::make_shared<MoveAction>();
		_decisionTree->SetFalseNode(moveAction);
		std::shared_ptr<MeleeAttackAction> attackAction = std::make_shared<MeleeAttackAction>(100.f, 1.f, 0.75f, _attackDamage);
		_decisionTree->SetTrueNode(attackAction);
	
}
void BoarBoss::MakeDecision() {
	_decisionMade = _decisionTree->MakeDecision(*this);
	if (_decisionMade->GetNodeType() == NodeType::ActionNode) {
		_decisionTreeAction = std::static_pointer_cast<Action>(_decisionMade);
	}
	//Node is not an action, recursion failed

}