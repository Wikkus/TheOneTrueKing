#pragma once
#include "collision.h"
#include "objectBase.h"
#include "sprite.h"
#include "vector2.h"

enum class ProjectileType {
	EnemyProjectile,
	PlayerProjectile,
	Count
};

class Projectile : public ObjectBase {
public:
	Projectile(ProjectileType damageType, const char* spritePath, unsigned int projectileDamage, unsigned int objectID);
	~Projectile();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	const ObjectType GetObjectType() const override;
	const unsigned int GetObjectID() const override;
	const Vector2<float> GetPosition() const override;
	const std::shared_ptr<Sprite> GetSprite() const override;
	
	const Circle GetCollider() const;
	const ProjectileType GetProjectileType() const;
	const unsigned int GetProjectileDamage() const;
	
	const float GetOrientation() const;
	
	void SetDirection(Vector2<float> direction);
	void SetTargetOrientation(float orientation);
	void SetTargetPosition(Vector2<float> position);

	void ActivateProjectile(float orientation, Vector2<float> direction, Vector2<float> position);
	void DeactivateProjectile();

private:
	Circle _circleCollider;

	const float _projectileSpeed = 200.f;
	const float _spriteCollisionOffset = 8.f;

	ProjectileType _projectileType = ProjectileType::Count;	

	unsigned int _projectileDamage;

	Vector2<float> _direction = Vector2<float>(0.f, 0.f);
};

