#pragma once
#include "formationManager.h"
#include "quadTree.h"
#include "vector2.h"

#include <vector>
#include <unordered_map>
#include <memory>

class EnemyBase;
class SteeringBehavior;
class Timer;
template<typename T> class ObjectPool;
template<typename T> class QuadTree;

enum class EnemyType;

enum class EnemyType {
	Boar,
	Human,
	Count
};

struct BehaviorData {
	std::vector<std::shared_ptr<ObjectBase>> queriedObjects;

	float rotation = 0.f;
	float targetOrientation = 0.f;

	float timeToTarget = 0.1f;

	Vector2<float> targetPosition = Vector2<float>{ 0.f, 0.f };
	Vector2<float> velocity = Vector2<float>{ 0.f, 0.f };

	//align data
	float angularSlowDownRadius = 0.f;
	float angularTargetRadius = 0.f;
	float maxAngularAcceleration = 0.f;
	float maxRotation = 0.f;

	//seek data
	float maxLinearAcceleration = 25.f;
	float maxSpeed = 50.f;
	float linearSlowDownRadius = 50.f;
	float linearTargetRadius = 10.f;

	//pursue data
	float maxPrediction = 0.f;
	Vector2<float> targetsVelocity = Vector2<float>{ 0.f, 0.f };

	//collision avoidance data
	float characterRadius = 0.f;

	//Obstacle avoidance data
	float avoidDistance = 0.f;
	float lookAhead = 0.f;

	//separation data
	float separationThreshold = 0.f;
	float attractionThreshold = 0.f;
	float decayCoefficient = 0.f;

	//wander data
	float wanderOffset = 0.f;
	float wanderRadius = 0.f;
	float wanderRate = 0.f;


};

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Init();
	void Update();
	void UpdateSurvival();
	void UpdateFormation();
	void Render();

	std::vector<std::shared_ptr<EnemyBase>> GetActiveEnemies();
	const std::vector<std::shared_ptr<FormationManager>>  GetFormationManagers() const;

	std::shared_ptr<WeaponComponent> AccessWeapon(WeaponType weaponType);

	void CreateNewEnemy(EnemyType enemyType, float orientation,
		Vector2<float> direction, Vector2<float> position);

	void CreateWeapon(WeaponType weaponType);

	void FormationEnemySpawner();
	void SpawnFormation(std::array<unsigned int, 2>  spawnCountPerRow, FormationType formationType);
	
	void SurvivalEnemySpawner();

	void SpawnEnemy(EnemyType enemyType, float orientation, 
		Vector2<float> direction, Vector2<float> position, WeaponType weaponType);

	void RemoveAllEnemies();
	void RemoveEnemy(EnemyType enemyType, unsigned int objectID);

	void Reset();

	void TakeDamage(unsigned int enemyIndex, unsigned int damageAmount);

	void UpdateQuadTree();

	const unsigned int GetWaveNumber() const;

	int BinarySearch(int low, int high, int objectID);

	int Partition(int start, int end);
	void QuickSort( int start, int end);

private:
	std::array<Vector2<float>, 4> _spawnPositions;

	std::shared_ptr<AnchorPoint> _latestAnchorPoint = nullptr;
	std::vector<std::shared_ptr<FormationManager>> _formationManagers;

	std::shared_ptr<Timer> _spawnTimer = nullptr;

	std::vector<std::shared_ptr<EnemyBase>> _activeEnemies;

	std::unordered_map<EnemyType, std::shared_ptr<ObjectPool<std::shared_ptr<EnemyBase>>>> _enemyPools;

	std::unordered_map<WeaponType, std::shared_ptr<ObjectPool<std::shared_ptr<WeaponComponent>>>> _weaponPools;

	unsigned int _currentSpawnAmount = 0;
	SlotAttackType _currentAttackType = SlotAttackType::Count;
	WeaponType _currentWeaponType = WeaponType::Count;
	Vector2<float> _currentSpawnPosition;
	Vector2<float> _currentSpawnDirection;

	unsigned int _formationsSpawned = 1;
	unsigned int _minCountSpawn = 9;
	unsigned int _minRowSpawn = 1;
	std::array<unsigned int, 2> _spawnCountPerRow = { 6, 1 };

	int _latestEnemyIndex = -1;

	unsigned int _enemyAmountLimit = 1000;
	unsigned int _weaponAmountLimit = 1000;
	unsigned int _numberOfEnemyTypes = 0;
	unsigned int _numberOfWeaponTypes = 0;
	unsigned int _spawnNumberOfEnemies = 5;
	unsigned int _waveNumber = 1;
};

