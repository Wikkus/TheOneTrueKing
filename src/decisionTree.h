#pragma once
#include "timerHandler.h"
#include "vector2.h"

#include <memory>
#include <vector>

class EnemyBase;
class ObjectBase;
class WeaponComponent;

enum class NodeType {
	DecisionNode,
	ActionNode,
	Count
};

class DecisionTreeNode : public std::enable_shared_from_this<DecisionTreeNode> {
public:
	DecisionTreeNode(std::shared_ptr<ObjectBase> owner);
	~DecisionTreeNode() {}

	virtual std::shared_ptr<DecisionTreeNode> MakeDecision();
	virtual std::shared_ptr<DecisionTreeNode> GetBranch();

	const NodeType GetNodeType() const;

protected:
	NodeType _nodeType = NodeType::Count;
	std::shared_ptr<EnemyBase> _owner;
};

#pragma region Actions
class Action : public DecisionTreeNode {
public:
	Action(std::shared_ptr<ObjectBase> owner);
	~Action() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;

	virtual bool ExecuteAction() { return false; }


};

class AttackAction : public Action {
public:
	AttackAction(std::shared_ptr<ObjectBase> owner) : Action(owner) {}
	~AttackAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;
	virtual bool ExecuteAction() override { return false; }

protected:
	std::shared_ptr<WeaponComponent> _weaponComponent = nullptr;
};
class DashAction : public AttackAction {
public:
	DashAction(std::shared_ptr<ObjectBase> owner);
	~DashAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;
	bool ExecuteAction() override;

private:
	std::shared_ptr<WeaponComponent> _tuskComponent = nullptr;

}; 
class EnergyBlastAction : public AttackAction {
public:
	EnergyBlastAction(std::shared_ptr<ObjectBase> owner);
	~EnergyBlastAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;
	bool ExecuteAction() override;

};
class WarstompAction : public AttackAction {
public:
	WarstompAction(std::shared_ptr<ObjectBase> owner);
	~WarstompAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;
	bool ExecuteAction() override;

};

class MoveAction : public Action {
public:
	MoveAction(std::shared_ptr<ObjectBase> owner) : Action(owner) {}
	~MoveAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision() override;
	bool ExecuteAction() override;
};

#pragma endregion

#pragma region Decisions
class Decision : public DecisionTreeNode {
public:
	Decision(std::shared_ptr<ObjectBase> owner);
	~Decision() {}

	virtual std::shared_ptr<DecisionTreeNode> GetBranch() override;
	virtual std::shared_ptr<DecisionTreeNode> MakeDecision() override;

	void SetFalseNode(std::shared_ptr<DecisionTreeNode> falseNode);
	void SetTrueNode(std::shared_ptr<DecisionTreeNode> trueNode);

protected:
	std::shared_ptr<DecisionTreeNode> _falseNode;
	std::shared_ptr<DecisionTreeNode> _trueNode;
	
};


class WithinRangeDecision : public Decision {
public:
	WithinRangeDecision(std::shared_ptr<ObjectBase> owner, const float& minRange, const float& maxRange);
	~WithinRangeDecision() {}

	std::shared_ptr<DecisionTreeNode> GetBranch() override;

	float TestValue(const Vector2<float>& position, const Vector2<float>& targetPosition);

private:
	float _minValue = 0.f;
	float _maxValue = 0.f;
	float _distance = 0.f;
};

class RandomDecision : public Decision {
public:
	RandomDecision(std::shared_ptr<ObjectBase> owner, const float& timeOut);
	~RandomDecision() {}

	bool TestValue();

private:
	std::shared_ptr<Timer> _timer;

	bool _currentDecision = false;

};
#pragma endregion

#pragma region MultiDecisions
class MultiDecision : public DecisionTreeNode {
public:
	MultiDecision(std::shared_ptr<ObjectBase> owner) : DecisionTreeNode(owner) {}
	~MultiDecision() {}

	std::shared_ptr<DecisionTreeNode> GetBranch() override;
	std::shared_ptr<DecisionTreeNode> MakeDecision() override;

	void AddNode(std::shared_ptr<DecisionTreeNode> node);

	virtual int TestValue();

protected:
	std::vector<std::shared_ptr<DecisionTreeNode>> _daughterNodes;

};

class RandomMultiDecision : public MultiDecision {
public:
	RandomMultiDecision(std::shared_ptr<ObjectBase> owner) : MultiDecision(owner) {}
	~RandomMultiDecision() {}

	int TestValue() override;
};
#pragma endregion