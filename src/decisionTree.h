#pragma once
#include <memory>
#include <vector>

#include "timerManager.h"
#include "vector2.h"

class EnemyBase;

enum class NodeType {
	DecisionNode,
	ActionNode,
	Count
};

class DecisionTreeNode : public std::enable_shared_from_this<DecisionTreeNode> {
public:
	DecisionTreeNode();
	~DecisionTreeNode() {}

	virtual std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner);
	virtual std::shared_ptr<DecisionTreeNode> GetBranch(EnemyBase& owner);

	const NodeType GetNodeType() const;

protected:
	NodeType _nodeType = NodeType::Count;

};

#pragma region Actions
class Action : public DecisionTreeNode {
public:
	Action();
	~Action() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	virtual bool ExecuteAction(EnemyBase& owner) { return false; }


};

class MeleeAttackAction : public Action {
public:
	MeleeAttackAction(float attackRange, float attackCooldown, float attackChargeTime, int attackDamage);
	~MeleeAttackAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	bool ExecuteAction(EnemyBase& owner) override; 
private:
	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;

	bool _isAttacking = false;
	float _attackRange = 0.f;
	int _attackDamage = 0;
};

class DashAction : public Action {
public:
	DashAction();
	~DashAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	bool ExecuteAction(EnemyBase& owner) override;
};

class MoveAction : public Action {
public:
	MoveAction();
	~MoveAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	bool ExecuteAction(EnemyBase& owner) override;
};

#pragma endregion

#pragma region Decisions
class Decision : public DecisionTreeNode {
public:
	Decision();
	~Decision() {}

	virtual std::shared_ptr<DecisionTreeNode> GetBranch(EnemyBase& owner) override;
	virtual std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	void SetFalseNode(std::shared_ptr<DecisionTreeNode> falseNode);
	void SetTrueNode(std::shared_ptr<DecisionTreeNode> trueNode);

protected:
	std::shared_ptr<DecisionTreeNode> _falseNode;
	std::shared_ptr<DecisionTreeNode> _trueNode;
	
};


class WithinRangeDecision : public Decision {
public:
	WithinRangeDecision(float maxRange, float minRange);
	~WithinRangeDecision() {}

	std::shared_ptr<DecisionTreeNode> GetBranch(EnemyBase& owner) override;

	float TestValue(Vector2<float> position, Vector2<float> targetPosition);

private:
	float _minValue = 0.f;
	float _maxValue = 0.f;
	float _distance = 0.f;
};

class RandomDecision : public Decision {
public:
	RandomDecision(float timeOut);
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
	MultiDecision();
	~MultiDecision() {}

	std::shared_ptr<DecisionTreeNode> GetBranch(EnemyBase& owner) override;
	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	void AddNode(std::shared_ptr<DecisionTreeNode> node);

	virtual int TestValue();

protected:
	std::vector<std::shared_ptr<DecisionTreeNode>> _daughterNodes;

};

class RandomMultiDecision : public MultiDecision {
public:
	RandomMultiDecision();
	~RandomMultiDecision() {}

	int TestValue() override;
};
#pragma endregion