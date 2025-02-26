#include "decisionTree.h"

#include "universalFunctions.h"
#include "gameEngine.h"
#include "enemyBase.h"
#include "playerCharacter.h"
#include "collision.h"

DecisionTreeNode::DecisionTreeNode() {
}

std::shared_ptr<DecisionTreeNode> DecisionTreeNode::MakeDecision(EnemyBase& owner) {
	return std::shared_ptr<DecisionTreeNode>();
}
std::shared_ptr<DecisionTreeNode> DecisionTreeNode::GetBranch(EnemyBase& owner) {
	return std::shared_ptr<DecisionTreeNode>();
}

const NodeType DecisionTreeNode::GetNodeType() const {
	return _nodeType;
}

Action::Action() {
	_nodeType = NodeType::ActionNode;
}

std::shared_ptr<DecisionTreeNode> Action::MakeDecision(EnemyBase& owner) {	
	return shared_from_this();
}

MeleeAttackAction::MeleeAttackAction(const float& attackRange, const float& attackCooldown, 
	const float& attackChargeTime, const int& attackDamage) {
	_attackCooldownTimer = timerManager->CreateTimer(attackCooldown);
	_chargeAttackTimer = timerManager->CreateTimer(attackChargeTime);
	_attackDamage = attackDamage;
	_attackRange = attackRange;
}

std::shared_ptr<DecisionTreeNode> MeleeAttackAction::MakeDecision(EnemyBase& owner) {
	return shared_from_this();
}

bool MeleeAttackAction::ExecuteAction(EnemyBase& owner) {
	if (_attackCooldownTimer->GetTimerActive()) {
		return false;
	}
	if (_chargeAttackTimer->GetTimerActive()) {
		owner.UpdateAngularMovement();
	}
	if (_isAttacking && _chargeAttackTimer->GetTimerFinished()) {
		if (universalFunctions->IsInDistance(owner.GetCurrentTarget()->GetPosition(), owner.GetPosition(), _attackRange)) {
			owner.GetCurrentTarget()->TakeDamage(_attackDamage);
		}
		owner.SetVelocity({ 0.f, 0.f });
		owner.SetRotation(0.f);
		_isAttacking = false;
		_attackCooldownTimer->ResetTimer();
		_chargeAttackTimer->DeactivateTimer();
		return true;

	} else if (!_isAttacking) {
		_chargeAttackTimer->ResetTimer();
		_isAttacking = true;
	}
	return false;
}

DashAction::DashAction() {}

std::shared_ptr<DecisionTreeNode> DashAction::MakeDecision(EnemyBase& owner) {
	return std::shared_ptr<DecisionTreeNode>();
}

bool DashAction::ExecuteAction(EnemyBase& owner) {
	return true;
}

MoveAction::MoveAction() {}

std::shared_ptr<DecisionTreeNode> MoveAction::MakeDecision(EnemyBase& owner) {
	return shared_from_this();
}

bool MoveAction::ExecuteAction(EnemyBase& owner) {
	owner.UpdateMovement();
	return true;
}


Decision::Decision() {
	_nodeType = NodeType::DecisionNode;
}

std::shared_ptr<DecisionTreeNode> Decision::GetBranch(EnemyBase& owner) {
	return std::shared_ptr<DecisionTreeNode>();
}

std::shared_ptr<DecisionTreeNode> Decision::MakeDecision(EnemyBase& owner) {
	return GetBranch(owner)->MakeDecision(owner);
}

void Decision::SetFalseNode(std::shared_ptr<DecisionTreeNode> falseNode) {
	_falseNode = falseNode;
}

void Decision::SetTrueNode(std::shared_ptr<DecisionTreeNode> trueNode) {
	_trueNode = trueNode;
}

WithinRangeDecision::WithinRangeDecision(float maxRange, float minRange) {
	_maxValue = maxRange;
	_minValue = minRange;
}

std::shared_ptr<DecisionTreeNode> WithinRangeDecision::GetBranch(EnemyBase& owner) {
	_distance = TestValue(owner.GetPosition(), owner.GetTargetPosition());
	if (_minValue <= _distance && _distance <= _maxValue) {
		return _trueNode;
	}
	return _falseNode;
}

float WithinRangeDecision::TestValue(const Vector2<float>& position, const Vector2<float>& targetPosition) {
	return Vector2<float>::distanceBetweenVectors(targetPosition, position);
}

RandomDecision::RandomDecision(const float& timeOut) {
	_timer = timerManager->CreateTimer(timeOut);
}

bool RandomDecision::TestValue() {
	if (_timer->GetTimerActive()) {
		return _currentDecision;
	} else {
		_currentDecision = universalFunctions->RandomBinomialInt(0, 1);
		_timer->ResetTimer();
	}
	return _currentDecision;
}

MultiDecision::MultiDecision() {}

std::shared_ptr<DecisionTreeNode> MultiDecision::GetBranch(EnemyBase& owner) {
	return _daughterNodes[TestValue()];
}

std::shared_ptr<DecisionTreeNode> MultiDecision::MakeDecision(EnemyBase& owner) {
	return GetBranch(owner)->MakeDecision(owner);
}

void MultiDecision::AddNode(std::shared_ptr<DecisionTreeNode> node) {
	_daughterNodes.emplace_back(node);
}

int MultiDecision::TestValue() { 
	return 0;
}

RandomMultiDecision::RandomMultiDecision() {}

int RandomMultiDecision::TestValue() {
	return universalFunctions->RandomBinomialInt(0, _daughterNodes.size() - 1);
}


