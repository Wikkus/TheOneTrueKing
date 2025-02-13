#pragma once
#include "collision.h"
#include "objectBase.h"
#include "sprite.h"
#include "vector2.h"

enum class ProjectileType {
	Energyblast,
	EnemyFireball,
	PlayerFireball,
	Count
};

class Projectile : public ObjectBase {
public:
	Projectile(unsigned int objectID, ProjectileType projectileType, const char* spritePath);
	~Projectile();

	void Init() override;
	void Update() override;
	void Render() override;
	const std::shared_ptr<Collider> GetCollider() const override;
	void SetPosition(Vector2<float> position) override;

	const ProjectileType GetProjectileType() const;
	const unsigned int GetProjectileDamage() const;
	
	void SetDirection(Vector2<float> direction);

	void ActivateProjectile(float orientation, Vector2<float> direction, Vector2<float> position, unsigned int damage, float speed);
	void DeactivateProjectile();

private:
	std::shared_ptr<Circle> _circleCollider = nullptr;

	float _speed = 200.f;

	ProjectileType _projectileType = ProjectileType::Count;	

	unsigned int _damage = 30;
};

