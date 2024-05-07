#pragma once
#include "vector2.h"

#include <memory>

class Sprite;
class Timer;

enum class WeaponType {
	Sword,
	Staff,
	Count
};

class WeaponComponent {
public:
	WeaponComponent() {}
	~WeaponComponent() {}

	virtual void Render(Vector2<float> position, float orientation) = 0;

	virtual void Attack(Vector2<float> position, float orientation) = 0;

	const virtual bool GetIsAttacking() const = 0;

	const virtual float GetAttackDamage() const = 0;
	const virtual float GetAttackRange() const = 0;

	const virtual WeaponType GetWeaponType() const = 0;

protected:
	std::shared_ptr<Sprite> _sprite = nullptr;

	std::shared_ptr<Timer> _attackCooldownTimer = nullptr;
	std::shared_ptr<Timer> _chargeAttackTimer = nullptr;

	int _attackDamage = 0;
	float _attackRange = 0.f;
	float _attackTime = 1.f;

	float _attackDelay = 0.5f;

	bool _isAttacking = false;

	WeaponType _weaponType = WeaponType::Count;
};

class SwordComponent : public WeaponComponent {
public:
	SwordComponent();
	~SwordComponent() {}

	void Render(Vector2<float> position, float orientation) override;

	void Attack(Vector2<float> position, float orientation) override;

	const bool GetIsAttacking() const override;

	const float GetAttackDamage() const override;
	const float GetAttackRange() const override;

	const WeaponType GetWeaponType() const override;
};

class StaffComponent : public WeaponComponent {
public:
	StaffComponent();
	~StaffComponent() {}

	void Render(Vector2<float> position, float orientation) override;

	void Attack(Vector2<float> position, float orientation) override;

	const bool GetIsAttacking() const override;

	const float GetAttackDamage() const override;
	const float GetAttackRange() const override;

	const WeaponType GetWeaponType() const;
};