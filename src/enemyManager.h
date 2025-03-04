#pragma once
#include "formationHandler.h"
#include "managerBase.h"
#include "quadTree.h"
#include "universalFunctions.h"
#include "vector2.h"

class EnemyBase;
class SteeringBehavior;
class Timer;

template<typename T> class ObjectPool;
template<typename T> class QuadTree;

enum class EnemyType;
enum class WeaponType;

class EnemyManager : public ManagerBase {
public:
	EnemyManager();
	~EnemyManager();

	void Init() override;
	void Update() override;

	void UpdateBossRush();
	void UpdateSurvival();
	void UpdateFormation();

	void RemoveAllObjects() override;
	void RemoveObject(const unsigned int& objectID) override;
	void Reset() override;

	const std::vector<std::shared_ptr<FormationHandler>> GetFormationManagers() const;
	const unsigned int GetWaveNumber() const;

	void CreateNewEnemy(const EnemyType& enemyType);

	void BossSpawner();

	void FormationEnemySpawner();
	void SpawnFormation(const std::array<unsigned int, 2>&  spawnCountPerRow, const FormationType& formationType);
	
	void SurvivalEnemySpawner();

	std::shared_ptr<EnemyBase> SpawnEnemy(const EnemyType& enemyType, const float& orientation,
		const Vector2<float>& direction, const Vector2<float>& position, const WeaponType& weaponType);

	std::shared_ptr<EnemyBase> CastAsEnemy(std::shared_ptr<ObjectBase> currentObject);

private:
	std::shared_ptr<EnemyBase> _currentEnemy = nullptr;
	WeaponType _currentWeaponType = WeaponType::Count;
	unsigned int _weaponPicked = 0;

	std::array<Vector2<float>, 4> _spawnPositions;

	std::shared_ptr<AnchorPoint> _latestAnchorPoint = nullptr;
	std::vector<std::shared_ptr<FormationHandler>> _formationManagers;

	std::shared_ptr<Timer> _spawnTimer = nullptr;

	std::unordered_map<EnemyType, std::shared_ptr<ObjectPool<std::shared_ptr<EnemyBase>>>> _enemyPools;

	bool _spawnEnemy = false;

	unsigned int _currentSpawnAmount = 0;
	SlotAttackType _currentAttackType = SlotAttackType::Count;

	Vector2<float> _currentSpawnPosition;
	Vector2<float> _currentSpawnDirection;

	unsigned int _formationsSpawned = 1;
	unsigned int _minCountSpawn = 15;
	unsigned int _minRowSpawn = 1;
	std::array<unsigned int, 2> _spawnCountPerRow = { 9, 1 };

	unsigned int _enemyAmountLimit = 3000;
	unsigned int _numberOfEnemyTypes = 0;

	unsigned int _spawnNumberOfEnemies = 5;
	unsigned int _waveNumber = 1;

};

