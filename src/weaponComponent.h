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
	WeaponComponent();
	~WeaponComponent() {}

	void Init();
	void Update();
	void Render(Vector2<float> position, float orientation);

	virtual void Attack(Vector2<float> position) {}

	const virtual bool GetIsAttacking() const;

	const virtual float GetAttackRange() const;
	
	const virtual int GetAttackDamage() const;
	const virtual int GetHealthModifier() const;

	const virtual WeaponType GetWeaponType() const;

	virtual void DeactivateTimers();

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

	void Attack(Vector2<float> position) override;
};

class StaffComponent : public WeaponComponent {
public:
	StaffComponent();
	~StaffComponent();

	void Attack(Vector2<float> position) override;

private:
	float _projectileOrientation = 0;
	float _projectileSpeed = 200.f;
	Vector2<float> _projectileDirection = { 0.f, 0.f };
};

class SwordComponent : public WeaponComponent {
public:
	SwordComponent();
	~SwordComponent();

	void Attack(Vector2<float> position) override;	
};