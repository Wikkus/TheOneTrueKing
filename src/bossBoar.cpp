#include "bossBoar.h"
#include "debugDrawer.h"
#include "decisionTree.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "timerHandler.h"
#include "weaponManager.h"

#include <string>

BoarBoss::BoarBoss() : EnemyBase(EnemyType::Boss) {
	_sprite->Load(_bossBoarSpritePath);

	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->GetHeight() * 0.5f);

	_behaviorData.characterRadius = _sprite->GetHeight() * 0.5f;

	_maxHealth = 15000;
	_currentHealth = _maxHealth;

	_attackDamage = 300;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 1.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 50.f;
	_behaviorData.maxSpeed = 100.f;

	_behaviorData.separationThreshold = _behaviorData.characterRadius * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->AddSteeringBehavior(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehavior(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);

	_healthTextSprite = std::make_shared<TextSprite>();

	_behaviorData.linearTargetRadius = 50.f;
	_behaviorData.linearSlowDownRadius = 200.f;
}

BoarBoss::~BoarBoss() {}

void BoarBoss::Init() {
	_targetPosition = _currentTarget->GetPosition();

	_position = Vector2<float>(windowWidth * 0.9f, windowHeight * 0.3f);
	_direction = _targetPosition - _position;

	_healthTextSprite->Init(fontType, 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_healthTextSprite->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.9f));
	
	if (!_decisionTree) {
		CreateDecisionTree();
	}
	MakeDecision();
}

void BoarBoss::Update() {
	_targetPosition = playerCharacters.back()->GetPosition();
	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);

	if (_decisionTreeAction) {
		if (_decisionTreeAction->ExecuteAction()) {
			MakeDecision();
		}
	}
}

void BoarBoss::Render() {
	_sprite->RenderWithOrientation(0, _position, _orientation);
}

void BoarBoss::RenderText() {
	_healthTextSprite->Render();
}

void BoarBoss::TakeDamage(const int& damageAmount) {
	_currentHealth -= damageAmount;
	if (_currentHealth <= 0) {
		enemyManager->RemoveObject(_objectID);
	}
	_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
}

void BoarBoss::CreateDecisionTree() {
	_decisionTree = std::make_shared<WithinRangeDecision>(shared_from_this(), 0.f, 250.f);
		std::shared_ptr<MoveAction> moveAction = std::make_shared<MoveAction>(shared_from_this());
		_decisionTree->SetFalseNode(moveAction);
		std::shared_ptr<EnergyBlastAction> attackAction = std::make_shared<EnergyBlastAction>(shared_from_this());
		_decisionTree->SetTrueNode(attackAction);
}
void BoarBoss::MakeDecision() {
	_decisionMade = _decisionTree->MakeDecision();
	if (_decisionMade->GetNodeType() == NodeType::ActionNode) {
		_decisionTreeAction = std::static_pointer_cast<Action>(_decisionMade);	
	}
}