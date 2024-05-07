#pragma once
#include "collision.h"
#include "objectBase.h"
#include "sprite.h"
#include "textSprite.h"
#include "vector2.h"

class Timer;
class Projectile;

class PlayerCharacter : ObjectBase {
public:
	PlayerCharacter(float characterOrientation, unsigned int objectID, Vector2<float> characterPosition);
	~PlayerCharacter();

	void Init() override;
	void Update() override;
	void Render() override;

	void RenderText() override;

	const unsigned int GetObjectID() const override;
	const ObjectType GetObjectType() const override;

	const std::shared_ptr<Sprite> GetSprite() const override;
	const Vector2<float> GetPosition() const override;

	void ExecuteDeath();
	void FireProjectile();
	void Respawn();

	void TakeDamage(unsigned int damageAmount);

	const Circle GetCircleCollider() const;

	const float GetOrientation() const;	

	const int GetCurrentHealth() const;

private:
	void UpdateHealthRegen();
	void UpdateInput();
	void UpdateMovement();
	void UpdateTarget();

	const char* _kingSprite = "res/sprites/HumanKing.png";

	Circle _circleCollider;

	const float _attackDamage = 10;
	const float _movementSpeed = 100.f;

	const int _maxHealth = 1000;

	int _currentHealth = 0;

	std::shared_ptr<Projectile> _collidedProjectile = nullptr;

	std::shared_ptr<TextSprite> _healthTextSprite = nullptr;

	std::shared_ptr<Timer> _attackTimer = nullptr;
	std::shared_ptr<Timer> _regenerationTimer = nullptr;

	Vector2<float> _oldPosition = Vector2<float>(0.f, 0.f);
	Vector2<float> _direction = Vector2<float>(0.f, 0.f);
};

