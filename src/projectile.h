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
	Projectile(ProjectileType damageType, const char* spritePath, unsigned int objectID);
	~Projectile();

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	const ObjectType GetObjectType() const override;
	const unsigned int GetObjectID() const override;
	const Vector2<float> GetPosition() const override;
	const Vector2<float> GetVelocity() const override;
	const std::shared_ptr<Sprite> GetSprite() const override;
	
	const std::shared_ptr<Collider> GetCollider() const;
	const ProjectileType GetProjectileType() const;
	const unsigned int GetProjectileDamage() const;
	
	const float GetOrientation() const;
	
	void SetDirection(Vector2<float> direction);
	void SetOrientation(float orientation);
	void SetPosition(Vector2<float> position);

	void ActivateProjectile(float orientation, unsigned int projectileDamage, unsigned int projectileSpeed, Vector2<float> direction, Vector2<float> position);
	void DeactivateProjectile();

private:
	std::shared_ptr<Circle> _circleCollider = nullptr;

	float _projectileSpeed = 200.f;

	ProjectileType _projectileType = ProjectileType::Count;	

	unsigned int _projectileDamage = 30;
};

