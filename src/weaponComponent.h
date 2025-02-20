#pragma once
#include "objectBase.h"
#include "projectile.h"
#include "vector2.h"

#include <memory>

class Circle;
class Sprite;
class Timer;

enum class WeaponType {
	Shield,
	Staff,
	SuperStaff,
	Sword,
	Tusks,
	Count
};

class EnemyBase;

class WeaponComponent {
public:
	WeaponComponent();
	~WeaponComponent() {}

	virtual void Init(std::shared_ptr<ObjectBase> owner);
	void Update();
	void Render();

	virtual void Attack() {}

	const virtual bool GetIsAttacking() const;

	const virtual float GetAttackRange() const;
	
	const virtual int GetAttackDamage() const;
	const virtual int GetHealthModifier() const;

	const virtual WeaponType GetWeaponType() const;

	virtual void DeactivateTimers();

protected:
	std::shared_ptr<ObjectBase> _owner = nullptr;

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

	void Attack() override;
};

class StaffComponent : public WeaponComponent {
public:
	StaffComponent();
	~StaffComponent();

	void Init(std::shared_ptr<ObjectBase> owner) override;
	void Attack() override;

protected:
	ProjectileType _projectileType = ProjectileType::Count;

	bool _unlimitedRange = false;

	float _projectileOrientation = 0;
	float _projectileSpeed = 200.f;
	Vector2<float> _projectileDirection = { 0.f, 0.f };
};
class SuperStaffComponent : public StaffComponent {
public:
	SuperStaffComponent();
	~SuperStaffComponent();

	void Attack() override;

private:
	unsigned int _multiShotAmount = 5;
	
	float _multiShotAngle = 0.f;
	float _angleOffset = 0.25;

	Vector2<float> _multiShotDirection = 0.f;
};

class SwordComponent : public WeaponComponent {
public:
	SwordComponent();
	~SwordComponent();

	void Attack() override;	
};

class TusksComponent : public WeaponComponent {
public:
	TusksComponent();
	~TusksComponent();

	void Attack() override;

private:
	std::shared_ptr<Timer> _damageCooldown = nullptr;

	float _dashDistance = 0.f;
	float _dashSpeed = 300.f;

	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;
};