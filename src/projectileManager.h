#pragma once
#include "managerBase.h"
#include "projectile.h"

#include <unordered_map>
#include <vector>

template<typename T> class ObjectPool;

class EnemyBase;

class ProjectileManager : public ManagerBase {
public:
	ProjectileManager();
	~ProjectileManager();

	void Init() override;
	void Update() override;
	void Render() override;

	bool CheckCollision(ProjectileType projectileType, unsigned int projectileIndex);

	void CreateNewProjectile(ProjectileType projectileType);
	
	void SpawnProjectile(std::shared_ptr<ObjectBase> owner, ProjectileType projectileType, float orientation,
		Vector2<float> direction, Vector2<float> position, unsigned int damage, float speed);	
	
	void RemoveAllProjectiles();
	void RemoveProjectile(ProjectileType projectileType, unsigned int projectileIndex);

	void Reset() override;

private:
	std::shared_ptr<Projectile> CastAsProjectile(std::shared_ptr<ObjectBase> object);
	std::shared_ptr<Projectile> _currentProjectile = nullptr;

	std::unordered_map<ProjectileType, std::shared_ptr<ObjectPool<std::shared_ptr<Projectile>>>> _projectilePools;

	std::unordered_map<ProjectileType, const char*> _spritePaths;
	const char* _energyBlastSprite = "res/sprites/Energyblast.png";
	const char* _enemyProjectileSprite = "res/sprites/Fireball.png";
	const char* _playerProjectileSprite = "res/sprites/Arcaneball.png";
	
	unsigned int _projectileAmountLimit = 3000;

	int _latestProjectileIndex = -1;

	std::vector<std::shared_ptr<ObjectBase>> _objectsHit;

	//Quicksort variables
	int _pivot = 0;
	int _pivotIndex = 0;
	int _count = 0;
	int _i = 0;
	int _k = 0;
	int _partition = 0;
};