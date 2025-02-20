#pragma once
#include "collision.h"
#include "objectBase.h"
#include "sprite.h"
#include "textSprite.h"
#include "vector2.h"

class Timer;
class Projectile;
class WeaponComponent;

class PlayerCharacter : public ObjectBase {
public:
	PlayerCharacter(float characterOrientation, unsigned int objectID, Vector2<float> characterPosition);
	~PlayerCharacter();

	void Init() override;
	void Update() override;
	void Render() override;

	void RenderText() override;

	const std::shared_ptr<Collider> GetCollider() const override;

	void ExecuteDeath();
	void Respawn();

	void TakeDamage(unsigned int damageAmount) override;

	const int GetCurrentHealth() const;

private:
	void UpdateHealthRegen();
	void UpdateInput();
	void UpdateMovement();
	void UpdateTarget();

	const char* _kingSprite = "res/sprites/HumanKing.png";
	std::shared_ptr<ObjectBase> _dummyTarget = nullptr;
	std::shared_ptr<WeaponComponent> _weaponComponent = nullptr;

	float _movementSpeed = 150.f;
	float _regenerationCooldown = 0.5f;

	unsigned int _multiShotAmount = 3;
	float _multiShotAngle = 0.f;
	Vector2<float> _multiShotDirection = 0.f;

	std::shared_ptr<TextSprite> _healthTextSprite = nullptr;

	std::shared_ptr<Timer> _regenerationTimer = nullptr;

	Vector2<float> _oldPosition = Vector2<float>(0.f, 0.f);
};

