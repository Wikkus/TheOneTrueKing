#pragma once
#include <memory>
#include <vector>

#include "vector2.h"

class EnemyBase;

enum class NodeType {
	DecisionNode,
	AttackNode,
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
	NodeType _nodeType;

};

class Action : public DecisionTreeNode {
public:
	Action();
	~Action() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	virtual bool ExecuteAction(EnemyBase& owner);

protected:
	bool _acitonFinished = false;

private:


};

class AttackAction : public Action {
public:
	AttackAction();
	~AttackAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	bool ExecuteAction(EnemyBase& owner) override;

private:

};

class MoveAction : public Action {
public:
	MoveAction();
	~MoveAction() {}

	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	bool ExecuteAction(EnemyBase& owner) override;

private:

};

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
class MultiDecision : public DecisionTreeNode {
public:
	MultiDecision();
	~MultiDecision() {}

	std::shared_ptr<DecisionTreeNode> GetBranch(EnemyBase& owner) override;
	std::shared_ptr<DecisionTreeNode> MakeDecision(EnemyBase& owner) override;

	int TestValue();

private:
	std::vector<std::shared_ptr<DecisionTreeNode>> _daughterNodes;

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