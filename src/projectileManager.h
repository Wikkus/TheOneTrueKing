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

	const char* GetEnemyProjectileSprite() const;
	const char* GetPlayerProjectileSprite() const;

	void CreateNewProjectile(ProjectileType projectileType, const char* spritePath, float orientation, unsigned int projectileDamage,
		Vector2<float> direction, Vector2<float> position);
	
	void SpawnProjectile(ProjectileType projectileType, const char* spritePath, float orientation, unsigned int projectileDamage, Vector2<float> direction, Vector2<float> position);
	
	void QuickSort(int start, int end);
	
	void RemoveAllProjectiles();
	void RemoveProjectile(ProjectileType projectileType, unsigned int projectileIndex);

	void UpdateQuadTree();

	int BinarySearch(int low, int high, int targetID);

	int Partition(int start, int end);

private:
	std::unordered_map<ProjectileType, std::shared_ptr<ObjectPool<std::shared_ptr<Projectile>>>> _projectilePools;
	std::vector<std::shared_ptr<Projectile>> _activeProjectiles;

	const char* _enemyProjectileSprite = "res/sprites/Fireball.png";
	const char* _playerProjectileSprite = "res/sprites/Arcaneball.png";
	
	unsigned int _projectileAmountLimit = 2000;
	unsigned int _numberOfProjectileTypes = 0;

	unsigned int _lastProjectileID = 0;
	int _latestProjectileIndex = -1;

	std::vector<std::shared_ptr<ObjectBase>> _objectsHit;
	std::shared_ptr<EnemyBase> _enemyHit = nullptr;
};