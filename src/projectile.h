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
	Projectile(ProjectileType projectileType, const char* spritePath);
	~Projectile();
	
	void Init() override;
	void Update() override;
	void Render() override;
	const std::shared_ptr<Collider> GetCollider() const override;
	const std::shared_ptr<ObjectBase> GetOwner() const;
	void SetPosition(Vector2<float> position) override;

	const ProjectileType GetProjectileType() const;

	const int GetDamage() const override;
	
	void SetDirection(Vector2<float> direction);

	void ActivateProjectile(std::shared_ptr<ObjectBase> owner, float orientation, Vector2<float> direction, Vector2<float> position, int damage, float speed);
	void DeactivateObject() override;

private:
	std::shared_ptr<ObjectBase> _owner = nullptr;

	ProjectileType _projectileType = ProjectileType::Count;
	
	float _speed = 200.f;
	int _damage = 30;
};

