#include "decisionTree.h"

#include "dataStructuresAndMethods.h"
#include "gameEngine.h"
#include "enemyBase.h"
#include "playerCharacter.h"
#include "collision.h"

DecisionTreeNode::DecisionTreeNode() {}

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


void Action::SetActionFinished(bool actionFinished) {
	_actionFinished = actionFinished;
}

const bool Action::GetActionFinished() const {
	return _actionFinished;
}

AttackAction::AttackAction() {}

std::shared_ptr<DecisionTreeNode> AttackAction::MakeDecision(EnemyBase& owner) {
	return shared_from_this();
}

void AttackAction::ExecuteAction(EnemyBase& owner) {
	owner.HandleAttack();
}

MoveAction::MoveAction() {
	_moveDuration = timerManager->CreateTimer(1.f);
}

std::shared_ptr<DecisionTreeNode> MoveAction::MakeDecision(EnemyBase& owner) {
	_moveDuration->ResetTimer();
	return shared_from_this();
}

void MoveAction::ExecuteAction(EnemyBase& owner) {
	if (IsInDistance(owner.GetBehaviorData().targetPosition, owner.GetPosition(), owner.GetAttackRange() - 5.f) 
		|| _moveDuration->GetTimerFinished()) {
		_actionFinished = true;
		_moveDuration->DeactivateTimer();
	}
	owner.UpdateMovement();
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
	_distance = TestValue(owner.GetPosition(), owner.GetBehaviorData().targetPosition);
	if (_minValue <= _distance && _distance <= _maxValue) {
		return _trueNode;
	}
	return _falseNode;
}

float WithinRangeDecision::TestValue(Vector2<float> position, Vector2<float> targetPosition) {
	return Vector2<float>::distanceBetweenVectors(targetPosition, position);
}

RandomDecision::RandomDecision() {}

bool RandomDecision::TestValue() {
	if (frameNumber > _lastFame + 1 || frameNumber >= _timeOutFrame) {	
		_currentDecision = RandomBinomalInt(0, 1);
		
		_timeOutFrame = frameNumber + _timeOut;
	}
	_lastFame = frameNumber;
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
	return RandomBinomalInt(0, _daughterNodes.size() - 1);
}
