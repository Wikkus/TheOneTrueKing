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
	BoarBoss(unsigned int objectID, EnemyType enemyType);
	~BoarBoss();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	bool TakeDamage(unsigned int damageAmount) override;

	const std::shared_ptr<Collider> GetCollider() const override;
	const EnemyType GetEnemyType() const override;
	const ObjectType GetObjectType() const override;

	const float GetOrientation() const override;
	const float GetRotation() const override;

	const int GetCurrentHealth() const override;
	const unsigned int GetObjectID() const override;

	const std::shared_ptr<Sprite> GetSprite() const override;

	const int GetFormationIndex() const override;

	const BehaviorData GetBehaviorData() const override;

	const Vector2<float> GetPosition() const override;
	const Vector2<float> GetVelocity() const override;

	const std::vector<std::shared_ptr<ObjectBase>> GetQueriedObjects() const override;

	const std::shared_ptr<WeaponComponent> GetWeaponComponent() const override;
	
	bool HandleAttack() override;
	bool UpdateMovement() override;

	void ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) override;
	void DeactivateEnemy() override;
	

	void SetFormationIndex(int formationIndex) override;
	void SetPosition(Vector2<float> position) override;
	void SetTargetPosition(Vector2<float> targetPosition) override;
	void SetTargetOrientation(float targetOrientation) override;
	void SetVelocity(Vector2<float> velocity) override;

private:
	bool MakeDecision();
	void CreateDecisionTree();

	const char* _bossBoarSprite = "res/sprites/BoarBoss.png";
	

	float _dashDistance = 0.f;
	float _dashSpeed = 300.f;

	float _attackRange = 0.f;
	int _attackDamage = 0;

	bool _damagedPlayer = false;
	bool _isAttacking = false;

	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;

	std::shared_ptr<TextSprite> _healthTextSprite = nullptr;
	
	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;

	std::shared_ptr<Action> _decisionTreeAction = nullptr;
	std::shared_ptr<Decision> _decisionTree = nullptr;
	std::shared_ptr<DecisionTreeNode> _decisionMade = nullptr;
};

