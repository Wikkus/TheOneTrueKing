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
	Projectile(const ProjectileType& projectileType, const char* spritePath);
	~Projectile();
	
	void Init() override;
	void Update() override;
	void Render() override;

	const std::shared_ptr<Collider> GetCollider() const override;
	const std::shared_ptr<ObjectBase> GetOwner() const;
	void SetPosition(const Vector2<float>& position) override;

	const ProjectileType GetProjectileType() const;

	const int GetDamage() const override;
	
	void SetDirection(const Vector2<float>& direction);

	void ActivateProjectile(std::shared_ptr<ObjectBase> owner, const float& orientation, const Vector2<float>& direction, 
		const Vector2<float>& position, const int& damage, const float& speed);
	void DeactivateObject() override;

	void QueryObjects() override;

private:
	std::shared_ptr<ObjectBase> _owner = nullptr;

	ProjectileType _projectileType = ProjectileType::Count;
	
	float _speed = 200.f;
	int _damage = 30;
};

