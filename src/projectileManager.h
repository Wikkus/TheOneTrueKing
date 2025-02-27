#pragma once
#include "managerBase.h"
#include "projectile.h"

template<typename T> class ObjectPool;

class EnemyBase;

class ProjectileManager : public ManagerBase {
public:
	ProjectileManager();
	~ProjectileManager();

	void Init() override;
	void Update() override;

	bool CheckCollision(const ProjectileType& projectileType, const unsigned int& objectID);

	void CreateNewProjectile(const ProjectileType& projectileType);
	
	std::shared_ptr<Projectile> SpawnProjectile(std::shared_ptr<ObjectBase> owner, const ProjectileType& projectileType, 
		const float& orientation, const Vector2<float>& direction, const Vector2<float>& position, const unsigned int& damage, const float& speed);	
	
	void RemoveAllObjects() override;
	void RemoveObject(const unsigned int& objectID) override;

private:
	std::shared_ptr<Projectile> CastAsProjectile(std::shared_ptr<ObjectBase> object);
	std::shared_ptr<Projectile> _currentProjectile = nullptr;

	std::unordered_map<ProjectileType, std::shared_ptr<ObjectPool<std::shared_ptr<Projectile>>>> _projectilePools;
	std::vector<std::shared_ptr<ObjectBase>> _objectsHit;

	std::unordered_map<ProjectileType, const char*> _spritePaths;
	const char* _energyBlastSprite = "res/sprites/Energyblast.png";
	const char* _enemyProjectileSprite = "res/sprites/Fireball.png";
	const char* _playerProjectileSprite = "res/sprites/Arcaneball.png";
	
	unsigned int _projectileAmountLimit = 3000;

	bool _projectileHit = false;

};