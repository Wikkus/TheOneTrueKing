#pragma once
#include "projectile.h"

#include <unordered_map>
#include <vector>

template<typename T> class ObjectPool;

class EnemyBase;

class ProjectileManager {
public:
	ProjectileManager();
	~ProjectileManager();

	void Init();
	void Update();
	void Render();

	bool CheckCollision(ProjectileType projectileType, unsigned int projectileIndex);

	const std::shared_ptr<ObjectPool<std::shared_ptr<Projectile>>> GetProjectilePool(ProjectileType projectileType);

	void CreateNewProjectile(ProjectileType projectileType);
	
	void SpawnProjectile(std::shared_ptr<ObjectBase> owner, ProjectileType projectileType, float orientation,
		Vector2<float> direction, Vector2<float> position, unsigned int damage, float speed);
	
	void QuickSort(int start, int end);
	
	void RemoveAllProjectiles();
	void RemoveProjectile(ProjectileType projectileType, unsigned int projectileIndex);

	void Reset();

	void InsertProjectilesQuadtree();

	int BinarySearch(int low, int high, int targetID);

	int Partition(int start, int end);

private:
	std::unordered_map<ProjectileType, std::shared_ptr<ObjectPool<std::shared_ptr<Projectile>>>> _projectilePools;
	std::vector<std::shared_ptr<Projectile>> _activeProjectiles;


	std::unordered_map<ProjectileType, const char*> _spritePaths;
	const char* _energyBlastSprite = "res/sprites/Energyblast.png";
	const char* _enemyProjectileSprite = "res/sprites/Fireball.png";
	const char* _playerProjectileSprite = "res/sprites/Arcaneball.png";
	
	unsigned int _projectileAmountLimit = 3000;

	int _latestProjectileIndex = -1;

	std::vector<std::shared_ptr<ObjectBase>> _objectsHit;
};