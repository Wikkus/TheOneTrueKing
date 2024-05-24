#pragma once
#include "vector2.h"

#include <memory>

class Sprite;
class Timer;

enum class WeaponType {
	Shield,
	Staff,
	Sword,
	Count
};

class EnemyBase;

class WeaponComponent {
public:
	WeaponComponent() {}
	~WeaponComponent() {}

	virtual void Render(Vector2<float> position, float orientation) = 0;

	virtual void Attack(Vector2<float> position, float orientation) = 0;

	virtual bool AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) = 0;

	const virtual bool GetIsAttacking() const = 0;

	const virtual float GetAttackRange() const = 0;
	
	const virtual int GetAttackDamage() const = 0;
	const virtual int GetHealthModifier() const = 0;

	const virtual WeaponType GetWeaponType() const = 0;

	virtual void DeactivateTimers() = 0;

protected:
	std::shared_ptr<Sprite> _sprite = nullptr;

	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;
	
	bool _isAttacking = false;

	float _attackRange = 0.f;
	float _attackTime = 1.f;
	float _attackDelay = 0.5f;

	int _attackDamage = 0;
	int _healthModifier = 0;

	WeaponType _weaponType = WeaponType::Count;
};

class ShieldComponent : public WeaponComponent {
public:
	ShieldComponent();
	~ShieldComponent();

	void Render(Vector2<float> position, float orientation) override;

	void Attack(Vector2<float> position, float orientation) override;

	bool AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) override;
	
	const bool GetIsAttacking() const override;

	const float GetAttackRange() const override;

	const int GetAttackDamage() const override;
	const int GetHealthModifier() const override;

	const WeaponType GetWeaponType() const override;

	void DeactivateTimers() override;
};

class StaffComponent : public WeaponComponent {
public:
	StaffComponent();
	~StaffComponent();

	void Render(Vector2<float> position, float orientation) override;

	void Attack(Vector2<float> position, float orientation) override;

	bool AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) override;

	const bool GetIsAttacking() const override;

	const float GetAttackRange() const override;

	const int GetAttackDamage() const override;
	const int GetHealthModifier() const override;

	const WeaponType GetWeaponType() const;

	void DeactivateTimers() override;

private:
	float _projectileSpeed = 200.f;
};

class SwordComponent : public WeaponComponent {
public:
	SwordComponent();
	~SwordComponent();

	void Render(Vector2<float> position, float orientation) override;

	void Attack(Vector2<float> position, float orientation) override;
	
	bool AtTargetDistance(Vector2<float> position, Vector2<float> targetPosition, float distance, bool isInDistance) override;

	const bool GetIsAttacking() const override;

	const float GetAttackRange() const override;

	const int GetAttackDamage() const override;
	const int GetHealthModifier() const override;

	const WeaponType GetWeaponType() const override;

	void DeactivateTimers() override;

};