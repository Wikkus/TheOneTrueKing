#pragma once
#include "formationManager.h"
#include "managerBase.h"
#include "quadTree.h"
#include "universalFunctions.h"
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
	Boss,
	Boar,
	Human,
	Count
};

struct BehaviorData {
	float targetOrientation = 0.f;

	float timeToTarget = 0.1f;

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

class EnemyManager : public ManagerBase {
public:
	EnemyManager();
	~EnemyManager();

	void Init();
	void Update();

	void UpdateBossRush();
	void UpdateSurvival();
	void UpdateFormation();

	void Render();
	void RenderText();

	const std::vector<std::shared_ptr<FormationManager>>  GetFormationManagers() const;
	const unsigned int GetWaveNumber() const;

	std::shared_ptr<WeaponComponent> AccessWeapon(WeaponType weaponType);
	
	void CreateNewEnemy(EnemyType enemyType, float orientation,
		Vector2<float> direction, Vector2<float> position);

	void CreateWeapon(WeaponType weaponType);

	void BossSpawner();

	void FormationEnemySpawner();
	void SpawnFormation(std::array<unsigned int, 2>  spawnCountPerRow, FormationType formationType);
	
	void SurvivalEnemySpawner();

	void SpawnEnemy(EnemyType enemyType, float orientation, 
		Vector2<float> direction, Vector2<float> position, WeaponType weaponType);

	void RemoveAllEnemies();
	void RemoveEnemy(EnemyType enemyType, unsigned int objectID);

	void Reset();


private:
	std::shared_ptr<EnemyBase> CastAsEnemy(std::shared_ptr<ObjectBase> currentObject);
	std::shared_ptr<EnemyBase> _currentEnemy = nullptr;

	std::array<Vector2<float>, 4> _spawnPositions;

	std::shared_ptr<AnchorPoint> _latestAnchorPoint = nullptr;
	std::vector<std::shared_ptr<FormationManager>> _formationManagers;

	std::shared_ptr<Timer> _spawnTimer = nullptr;

	std::unordered_map<EnemyType, std::shared_ptr<ObjectPool<std::shared_ptr<EnemyBase>>>> _enemyPools;
	std::unordered_map<WeaponType, std::shared_ptr<ObjectPool<std::shared_ptr<WeaponComponent>>>> _weaponPools;

	bool _spawnEnemy = false;

	unsigned int _currentSpawnAmount = 0;
	SlotAttackType _currentAttackType = SlotAttackType::Count;
	WeaponType _currentWeaponType = WeaponType::Count;
	Vector2<float> _currentSpawnPosition;
	Vector2<float> _currentSpawnDirection;

	unsigned int _formationsSpawned = 1;
	unsigned int _minCountSpawn = 9;
	unsigned int _minRowSpawn = 1;
	std::array<unsigned int, 2> _spawnCountPerRow = { 6, 1 };

	unsigned int _enemyAmountLimit = 1000;
	unsigned int _weaponAmountLimit = 2000;
	unsigned int _numberOfEnemyTypes = 0;
	unsigned int _numberOfWeaponTypes = 0;
	unsigned int _spawnNumberOfEnemies = 5;
	unsigned int _waveNumber = 1;

	int _targetEnemyIndex = -1;

	//Quicksort variables
	int _pivot = 0;
	int _pivotIndex = 0;
	int _count = 0;
	int _i = 0;
	int _k = 0;
	int _partition = 0;
};

