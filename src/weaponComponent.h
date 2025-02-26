#pragma once
#include "objectBase.h"
#include "projectile.h"
#include "vector2.h"

#include <memory>

class Circle;
class Sprite;
class Timer;

class EnemyBase;

enum class WeaponType {
	Shield,
	Staff,
	SuperStaff,
	Sword,
	Tusks,
	Count
};

class WeaponComponent : public ObjectBase {
public:
	WeaponComponent();
	~WeaponComponent() {}

	virtual void Init() override;
	void Update() override;

	virtual void Attack() {}

	const virtual bool GetIsAttacking() const;

	const virtual float GetAttackRange() const;
	
	const virtual int GetAttackDamage() const;
	const virtual int GetHealthModifier() const;

	const virtual WeaponType GetWeaponType() const;

	void DeactivateTimers();
	void ResetTimers();

	void SetOwner(std::shared_ptr<ObjectBase> owner);
	
	void DeactivateObject() override;

protected:
	std::shared_ptr<ObjectBase> _owner = nullptr;

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

private:
	const char* _path = "res/sprites/Shield.png";
};

class StaffComponent : public WeaponComponent {
public:
	StaffComponent();
	~StaffComponent();

	void Init() override;
	void Attack() override;

protected:
	const char* _path = "res/sprites/Staff.png";
	ProjectileType _projectileType = ProjectileType::Count;

	bool _unlimitedRange = false;
	float _projectileSpeed = 200.f;
};
class SuperStaffComponent : public StaffComponent {
public:
	SuperStaffComponent();
	~SuperStaffComponent();

	void Attack() override;

private:
	unsigned int _multiShotAmount = 3;
	
	float _multiShotAngle = 0.f;
	float _angleOffset = 0.25;

	Vector2<float> _multiShotDirection = 0.f;
};

class SwordComponent : public WeaponComponent {
public:
	SwordComponent();
	~SwordComponent();

	void Attack() override;	

private:
	const char* _path = "res/sprites/Sword.png";
};

class TusksComponent : public WeaponComponent {
public:
	TusksComponent();
	~TusksComponent();

	void Attack() override;

private:
	const char* _path = "res/sprites/Tusks.png";
	std::shared_ptr<Timer> _damageCooldown = nullptr;

	float _dashDistance = 0.f;
	float _dashSpeed = 300.f;

	Vector2<float> _dashDirection;
	Vector2<float> _dashStartPosition;
};