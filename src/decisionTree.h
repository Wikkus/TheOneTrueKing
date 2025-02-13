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

	virtual void ExecuteAction(EnemyBase& owner) {}

	void SetActionFinished(bool actionFinished);
	const bool GetActionFinished() const;


protected:

	bool _actionFinished = false;

};

class AttackAction : public Action {
public:
	AttackAction();
	~AttackAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	void ExecuteAction(EnemyBase& owner) override;

};

class MoveAction : public Action {
public:
	MoveAction();
	~MoveAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	void ExecuteAction(EnemyBase& owner) override;
private:
	std::shared_ptr<Timer> _moveDuration = nullptr;
	Vector2<float> _targetPos;

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
	RandomDecision();
	~RandomDecision() {}

	bool TestValue();


private:
	bool _currentDecision = false;
	int _lastFame = -1;

	int _timeOut = 1000;
	int _timeOutFrame = -1;

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