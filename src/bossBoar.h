#pragma once
#include "collision.h"
#include "decisionTree.h"
#include "enemyBase.h"
#include "sprite.h"
#include "textSprite.h"
#include "vector2.h"

#include <memory>

class Timer;

class BoarBoss : public EnemyBase {
public:
	BoarBoss();
	~BoarBoss();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	void TakeDamage(const int& damageAmount) override;

private:
	void CreateDecisionTree();
	void MakeDecision();
	const char* _bossBoarSpritePath = "res/sprites/BoarBoss.png";

	int _attackDamage = 0;

	std::shared_ptr<TextSprite> _healthTextSprite = nullptr;
	
	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;

	std::shared_ptr<Action> _decisionTreeAction = nullptr;
	std::shared_ptr<Decision> _decisionTree = nullptr;
	std::shared_ptr<DecisionTreeNode> _decisionMade = nullptr;
};

