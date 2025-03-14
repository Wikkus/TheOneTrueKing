#include "decisionTree.h"

#include "collision.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "universalFunctions.h"
#include "weaponComponent.h"
#include "weaponManager.h"

DecisionTreeNode::DecisionTreeNode(std::shared_ptr<ObjectBase> owner) {
	_owner = std::static_pointer_cast<EnemyBase>(owner);
}

std::shared_ptr<DecisionTreeNode> DecisionTreeNode::MakeDecision() {
	return std::shared_ptr<DecisionTreeNode>();
}
std::shared_ptr<DecisionTreeNode> DecisionTreeNode::GetBranch() {
	return std::shared_ptr<DecisionTreeNode>();
}

const NodeType DecisionTreeNode::GetNodeType() const {
	return _nodeType;
}

Action::Action(std::shared_ptr<ObjectBase> owner) : DecisionTreeNode(owner) {
	_nodeType = NodeType::ActionNode;
}

std::shared_ptr<DecisionTreeNode> Action::MakeDecision() {	
	return shared_from_this();
}

std::shared_ptr<DecisionTreeNode> AttackAction::MakeDecision() {
	return shared_from_this();
}

DashAction::DashAction(std::shared_ptr<ObjectBase> owner) : AttackAction(owner) {
	_weaponComponent = weaponManager->SpawnWeapon(WeaponType::Tusks, _owner);
	_weaponComponent->SetWeaponValues(_owner, false, 300, 300.f, 2.f, 1.f);
	_owner->SetWeaponComponent(_weaponComponent);
}

std::shared_ptr<DecisionTreeNode> DashAction::MakeDecision() {
	return std::shared_ptr<DecisionTreeNode>();
}

bool DashAction::ExecuteAction() {
	return _weaponComponent->UpdateAttack();
}

EnergyBlastAction::EnergyBlastAction(std::shared_ptr<ObjectBase> owner) : AttackAction(owner) {
	_weaponComponent = weaponManager->SpawnWeapon(WeaponType::Staff, _owner);
	_weaponComponent->SetWeaponValues(_owner, false, 300, 500.f, 2.f, 1.f);
	std::static_pointer_cast<StaffComponent>(_weaponComponent)->SetProjectileValues(ProjectileType::Energyblast, true, 350.f);
	_owner->SetWeaponComponent(_weaponComponent);
}

std::shared_ptr<DecisionTreeNode> EnergyBlastAction::MakeDecision() {
	return shared_from_this();
}

bool EnergyBlastAction::ExecuteAction() {
	return _weaponComponent->UpdateAttack();
}

WarstompAction::WarstompAction(std::shared_ptr<ObjectBase> owner) : AttackAction(owner) {
	_weaponComponent = weaponManager->SpawnWeapon(WeaponType::Warstomp, _owner);
	_weaponComponent->SetWeaponValues(_owner, false, 300, 300.f, 2.f, 1.f);
	_owner->SetWeaponComponent(_weaponComponent);
}

std::shared_ptr<DecisionTreeNode> WarstompAction::MakeDecision() {
	return shared_from_this();
}

bool WarstompAction::ExecuteAction() {
	return _weaponComponent->UpdateAttack();
}


std::shared_ptr<DecisionTreeNode> MoveAction::MakeDecision() {
	return shared_from_this();
}

bool MoveAction::ExecuteAction() {
	_owner->UpdateMovement();
	return true;
}

Decision::Decision(std::shared_ptr<ObjectBase> owner)  : DecisionTreeNode(owner) {
	_nodeType = NodeType::DecisionNode;
}

std::shared_ptr<DecisionTreeNode> Decision::GetBranch() {
	return std::shared_ptr<DecisionTreeNode>();
}

std::shared_ptr<DecisionTreeNode> Decision::MakeDecision() {
	return GetBranch()->MakeDecision();
}

void Decision::SetFalseNode(std::shared_ptr<DecisionTreeNode> falseNode) {
	_falseNode = falseNode;
}

void Decision::SetTrueNode(std::shared_ptr<DecisionTreeNode> trueNode) {
	_trueNode = trueNode;
}

WithinRangeDecision::WithinRangeDecision(std::shared_ptr<ObjectBase> owner, const float& minRange, const float& maxRange) : Decision(owner) {
	_minValue = minRange;
	_maxValue = maxRange;
}

std::shared_ptr<DecisionTreeNode> WithinRangeDecision::GetBranch() {
	_distance = TestValue(_owner->GetPosition(), _owner->GetTargetPosition());
	if (_minValue <= _distance && _distance <= _maxValue) {
		return _trueNode;
	}
	return _falseNode;
}

float WithinRangeDecision::TestValue(const Vector2<float>& position, const Vector2<float>& targetPosition) {
	return Vector2<float>::distanceBetweenVectors(targetPosition, position);
}

RandomDecision::RandomDecision(std::shared_ptr<ObjectBase> owner, const float& timeOut) : Decision(owner) {
	_timer = timerHandler->SpawnTimer(timeOut, false, false);
}

bool RandomDecision::TestValue() {
	if (_timer->GetIsActive()) {
		return _currentDecision;
	} else {
		_currentDecision = universalFunctions->RandomBinomialInt(0, 1);
		_timer->ResetTimer();
	}
	return _currentDecision;
}

std::shared_ptr<DecisionTreeNode> MultiDecision::GetBranch() {
	return _daughterNodes[TestValue()];
}

std::shared_ptr<DecisionTreeNode> MultiDecision::MakeDecision() {
	return GetBranch()->MakeDecision();
}

void MultiDecision::AddNode(std::shared_ptr<DecisionTreeNode> node) {
	_daughterNodes.emplace_back(node);
}

int MultiDecision::TestValue() { 
	return 0;
}

int RandomMultiDecision::TestValue() {
	return universalFunctions->RandomBinomialInt(0, _daughterNodes.size() - 1);
}
