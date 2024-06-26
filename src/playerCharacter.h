#pragma once
#include "collision.h"
#include "objectBase.h"
#include "sprite.h"
#include "textSprite.h"
#include "vector2.h"

class Timer;
class Projectile;

class PlayerCharacter : public ObjectBase {
public:
	PlayerCharacter(float characterOrientation, unsigned int objectID, Vector2<float> characterPosition);
	~PlayerCharacter();

	void Init() override;
	void Update() override;
	void Render() override;

	void RenderText() override;

	const unsigned int GetObjectID() const override;
	const std::shared_ptr<Collider> GetCollider() const override;
	const ObjectType GetObjectType() const override;

	const std::shared_ptr<Sprite> GetSprite() const override;
	const Vector2<float> GetPosition() const override;
	const Vector2<float> GetVelocity() const override;

	void ExecuteDeath();
	void FireProjectile();
	void Respawn();

	void TakeDamage(unsigned int damageAmount);

	const float GetOrientation() const;	

	const int GetCurrentHealth() const;

private:
	void UpdateHealthRegen();
	void UpdateInput();
	void UpdateMovement();
	void UpdateTarget();

	const char* _kingSprite = "res/sprites/HumanKing.png";

	std::shared_ptr<Circle> _circleCollider = nullptr;

	float _attackDamage = 50;
	float _movementSpeed = 100.f;
	float _projectileSpeed = 300;

	float _attackCooldown = 0.25f;
	float _regenerationTime = 0.5f;

	int _maxHealth = 1000;
	int _currentHealth = 0;

	unsigned int _multiShotAmount = 3;
	float _multiShotAngle = 0.f;
	Vector2<float> _multiShotDirection = 0.f;

	std::shared_ptr<Projectile> _collidedProjectile = nullptr;

	std::shared_ptr<TextSprite> _healthTextSprite = nullptr;

	std::shared_ptr<Timer> _attackTimer = nullptr;
	std::shared_ptr<Timer> _regenerationTimer = nullptr;

	Vector2<float> _oldPosition = Vector2<float>(0.f, 0.f);
};

