#include "enemyManager.h"

#include "bossBoar.h"
#include "enemyBase.h"
#include "enemyBoar.h"
#include "enemyHuman.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "searchSortAlgorithms.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "timerManager.h"
#include "weaponComponent.h"

EnemyManager::EnemyManager() {
	//Creates an unordered map with objectpool of the different enemy types
	_numberOfEnemyTypes = (unsigned int)EnemyType::Count;
	for (unsigned int i = 0; i < _numberOfEnemyTypes; i++) {
		_enemyPools[(EnemyType)i] = std::make_shared<ObjectPool<std::shared_ptr<EnemyBase>>>(_enemyAmountLimit);
	}
	_numberOfWeaponTypes = (unsigned int)WeaponType::Count;
	for (unsigned int i = 0; i < _numberOfWeaponTypes; i++) {
		_weaponPools[(WeaponType)i] = std::make_shared<ObjectPool<std::shared_ptr<WeaponComponent>>>(_weaponAmountLimit);
	}

	_spawnPositions = {
		Vector2<float>(windowWidth, windowHeight * 0.5f),
		Vector2<float>(windowWidth * 0.5f, windowHeight),
		Vector2<float>(0.f, windowHeight * 0.5f),
		Vector2<float>(windowWidth * 0.5f, 0.f)
	};
}

EnemyManager::~EnemyManager() {
	RemoveAllEnemies();
}

void EnemyManager::Init() {
	_spawnTimer = timerManager->CreateTimer(3.f);
	//Create all enemies in the object pool at the start of the project
	for (unsigned i = 0; i < _enemyAmountLimit / _numberOfEnemyTypes; i++) {
		for (unsigned int k = 0; k < _numberOfEnemyTypes; k++) {
			CreateNewEnemy(EnemyType(k), 0.f, {0.f, 0.f}, {-10000.f, -10000.f});
		}
	}
	for (unsigned i = 0; i < _weaponAmountLimit / _numberOfWeaponTypes; i++) {
		for (unsigned int k = 0; k < _numberOfWeaponTypes; k++) {
			CreateWeapon((WeaponType)k);
		}
	}
}

void EnemyManager::Update() {
	if (_spawnTimer->GetTimerFinished() && _activeObjects.size() < _enemyAmountLimit) {
		_spawnEnemy = true;
	}
	for (unsigned i = 0; i < _activeObjects.size(); i++) {
		_activeObjects[i]->Update();
		_activeObjects[i]->QueryObjects();
	}
}
void EnemyManager::UpdateBossRush() {
	if (_spawnEnemy && _activeObjects.size() <= 0) {
		BossSpawner();
		_spawnEnemy = false;
	}
}
void EnemyManager::UpdateFormation() {
	if (_spawnEnemy) {
		FormationEnemySpawner();
		_spawnEnemy = false;
	}
	for (unsigned int i = 0; i < _formationManagers.size(); i++) {
		_formationManagers[i]->UpdateSlots();
	}
}
void EnemyManager::UpdateSurvival() {
	if (_spawnEnemy) {
		SurvivalEnemySpawner();
		_spawnEnemy = false;
	}
}

void EnemyManager::Render() {
	for (unsigned i = 0; i < _activeObjects.size(); i++) {
		_activeObjects[i]->Render();
	}
}

void EnemyManager::RenderText() {
	for (unsigned i = 0; i < _activeObjects.size(); i++) {
		_activeObjects[i]->RenderText();
	}
}

const std::vector<std::shared_ptr<FormationManager>> EnemyManager::GetFormationManagers() const {
	return _formationManagers;
}

std::shared_ptr<WeaponComponent> EnemyManager::AccessWeapon(WeaponType weaponType) {
	if (_weaponPools[weaponType]->IsEmpty()) {
		CreateWeapon(weaponType);	
	}
	return _weaponPools[weaponType]->SpawnObject();
}

//Creates a specific enemy based on the enemyType enum
void EnemyManager::CreateNewEnemy(EnemyType enemyType, float orientation, Vector2<float> direction, Vector2<float> position) {
	switch (enemyType) {
	case EnemyType::Boar:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyBoar>(lastObjectID, enemyType));
		break;

	case EnemyType::Human:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyHuman>(lastObjectID, enemyType));
		break;
	
	default:
		break;
	}
}

void EnemyManager::CreateWeapon(WeaponType weaponType) {
	switch (weaponType) {
	case WeaponType::Shield:
		_weaponPools[weaponType]->PoolObject(std::make_shared<ShieldComponent>());
		break;
	case WeaponType::Staff:
		_weaponPools[weaponType]->PoolObject(std::make_shared<StaffComponent>());
		break;			
	case WeaponType::SuperStaff:
		_weaponPools[weaponType]->PoolObject(std::make_shared<SuperStaffComponent>());
		break;
	case WeaponType::Sword:
		_weaponPools[weaponType]->PoolObject(std::make_shared<SwordComponent>());
		break;
	case WeaponType::Tusks:
		_weaponPools[weaponType]->PoolObject(std::make_shared<TusksComponent>());
		break;
	default:
		break;
	}
}

void EnemyManager::BossSpawner() {
	_activeObjects.emplace_back(std::make_shared<BoarBoss>(lastObjectID, EnemyType::Boss));
	_activeObjects.back()->Init();
}

void EnemyManager::FormationEnemySpawner() {
	if (_waveNumber % 3 == 0) {
		_spawnCountPerRow[1] += 1;
		_spawnCountPerRow[0] = _minCountSpawn;
	}
	if (_waveNumber % 9 == 0 && _formationsSpawned < 4) {
		_formationsSpawned += 1;
		_spawnCountPerRow = { _minCountSpawn, _minRowSpawn };
	}
	for (unsigned int i = 0; i < _formationsSpawned; i++) {
		std::uniform_int_distribution dist{ 0, (int)FormationType::Count - 1 };
		_latestAnchorPoint = std::make_shared<AnchorPoint>();
		_latestAnchorPoint->position = _spawnPositions[i];
		_latestAnchorPoint->targetPosition = { windowWidth * 0.5f, windowHeight * 0.5f };
		_latestAnchorPoint->direction = _latestAnchorPoint->targetPosition - _latestAnchorPoint->position;
		_latestAnchorPoint->orientation = universalFunctions->VectorAsOrientation(_latestAnchorPoint->direction);
		SpawnFormation(_spawnCountPerRow, (FormationType)dist(randomEngine));
	}
	_spawnCountPerRow[0] += 2;
	_spawnTimer->DeactivateTimer();
}

void EnemyManager::SpawnFormation(std::array<unsigned int, 2>  spawnCountPerRow, FormationType formationType) {
	_formationManagers.emplace_back(std::make_shared<FormationManager>(formationType, spawnCountPerRow, _latestAnchorPoint, false));	
	_currentWeaponType = WeaponType::Count;

	for (unsigned int i = 0; i < (unsigned int)SlotAttackType::Count; i++) {
		_currentAttackType = (SlotAttackType)i;
		switch (_currentAttackType) {
		case SlotAttackType::Defender:
			_currentWeaponType = WeaponType::Shield;
			break;
 
		case SlotAttackType::Mage:
			_currentWeaponType = WeaponType::Staff;
			break;
		
		case SlotAttackType::Swordsman:
			_currentWeaponType = WeaponType::Sword;
			break;

		default:
			break;
		}
		_currentSpawnAmount = _formationManagers.back()->GetFormationPattern()->GetSlotsPerType()[_currentAttackType];
		for (unsigned int k = 0; k < _currentSpawnAmount; k++) {
			enemyManager->SpawnEnemy(EnemyType::Human, 0, Vector2<float>(0.f, 0.f), _latestAnchorPoint->position, _currentWeaponType);
			_currentEnemy = CastAsEnemy(_activeObjects.back());
			_currentEnemy->SetFormationIndex(_formationManagers.size() - 1);
			_formationManagers.back()->AddCharacter(_currentEnemy);
		}
	}
	_currentEnemy = nullptr;
}

void EnemyManager::SurvivalEnemySpawner() {
	std::uniform_int_distribution dist{ 0, 1 };
	_currentSpawnAmount = _spawnNumberOfEnemies + (_waveNumber * 4);
	_currentWeaponType = WeaponType::Count;

	for (unsigned int i = 0; i < _currentSpawnAmount; i++) {
		if (i < _currentSpawnAmount * 0.5f) {
			std::uniform_real_distribution<float> distY{ 0.f, windowHeight };
			if (dist(randomEngine) == 0) {
				_currentSpawnPosition.x = 0.f;
			} else {
				_currentSpawnPosition.x = windowWidth;
			}
			_currentSpawnPosition.y = distY(randomEngine);

		} else {
			std::uniform_real_distribution<float> distX{ 0.f, windowWidth };
			if (dist(randomEngine) == 0) {
				_currentSpawnPosition.y = 0.f;
			} else {
				_currentSpawnPosition.y = windowWidth;
			}
			_currentSpawnPosition.x = distX(randomEngine);
		}
		_currentSpawnDirection = (playerCharacters.back()->GetPosition() - _currentSpawnPosition).normalized();
		if (i % 3 == 0) {
			enemyManager->SpawnEnemy(EnemyType::Boar, universalFunctions->VectorAsOrientation(_currentSpawnDirection), _currentSpawnDirection, _currentSpawnPosition, WeaponType::Tusks);
		} else {	
			std::uniform_int_distribution decideWeapon{ 1, 2 };
			unsigned int weaponPicked = decideWeapon(randomEngine);
			switch (weaponPicked) {
			case 1:
				_currentWeaponType = WeaponType::Staff;
				break;
			case 2:
				_currentWeaponType = WeaponType::Sword;
				break;
			default:
				_currentWeaponType = WeaponType::Sword;
				break;
			}
			enemyManager->SpawnEnemy(EnemyType::Human, universalFunctions->VectorAsOrientation(_currentSpawnDirection), _currentSpawnDirection, _currentSpawnPosition, _currentWeaponType);
		}
	}
	_waveNumber++;
	_spawnTimer->ResetTimer();
}

//Spawn a specific enemy from the object pool. If the pool is empty, create a new enemy of that type
void EnemyManager::SpawnEnemy(EnemyType enemyType, float orientation, 
	Vector2<float> direction, Vector2<float> position, WeaponType weaponType) {
	if (_enemyPools[enemyType]->IsEmpty()) {
		CreateNewEnemy(enemyType, orientation, direction, position);
	}
	//Then add the enemy to the active enemies vector which is called in Update
	_activeObjects.emplace_back(_enemyPools[enemyType]->SpawnObject());
	CastAsEnemy(_activeObjects.back())->ActivateEnemy(orientation, direction, position, weaponType);
	_objectIDs.emplace_back(_activeObjects.back()->GetObjectID());
}

void EnemyManager::RemoveAllEnemies() {
	while (_activeObjects.size() > 0) {
		_currentEnemy = CastAsEnemy(_activeObjects.back());
		_currentEnemy->DeactivateObject();
		_enemyPools[_currentEnemy->GetEnemyType()]->PoolObject(_currentEnemy);
		_activeObjects.pop_back();
	}
	_currentEnemy = nullptr;
	_objectIDs.clear();
}

//Using Quicksort and Binary search to locate a specific enemy
void EnemyManager::RemoveEnemy(EnemyType enemyType, unsigned int objectID) {
	if (_activeObjects.empty()) {
		return;
	}

	//Sorts the active enemies vector based on their ID from lowest to highest
	QuickSort(_objectIDs, 0, _objectIDs.size() - 1);
	
	//Search through the sorted vector to find the enemy with a specific ID
	_targetEnemyIndex = quickSort->BinarySearch(_objectIDs, 0, _objectIDs.size() - 1, objectID);

	if (_targetEnemyIndex < 0) {
		return;
	}
	_currentEnemy = CastAsEnemy(_activeObjects[_targetEnemyIndex]);

	if (_formationManagers.size() > 0) {
		_formationManagers[_currentEnemy->GetFormationIndex()]->RemoveCharacter(_currentEnemy);
	}
	//Deactivate the enemy by setting its position to a far away place
	_activeObjects[_targetEnemyIndex]->DeactivateObject();

	//Adds the enemy to the object pool and place it at the back of the vector
	_enemyPools[enemyType]->PoolObject(_currentEnemy);
	std::swap(_activeObjects[_targetEnemyIndex], _activeObjects.back());
	std::swap(_objectIDs[_targetEnemyIndex], _objectIDs.back());

	//Removes the enemy from active enemies
	_activeObjects.pop_back();
	_objectIDs.pop_back();

	if (_activeObjects.size() <= 0) {
		_waveNumber++;
		_spawnTimer->ResetTimer();
		if (gameStateHandler->GetGameMode() == GameMode::Formation) {
			_formationManagers.clear();
			_latestAnchorPoint = nullptr;
		}
	}
	_currentEnemy = nullptr;
}

void EnemyManager::Reset() {
	_formationsSpawned = 1;
	_spawnCountPerRow = { _minCountSpawn, _minRowSpawn };
	_formationManagers.clear();
	_latestAnchorPoint = nullptr;
	_waveNumber = 1;
	RemoveAllEnemies();
	_spawnTimer->ResetTimer();
}

const unsigned int EnemyManager::GetWaveNumber() const {
	return _waveNumber;
}

std::shared_ptr<EnemyBase> EnemyManager::CastAsEnemy(std::shared_ptr<ObjectBase> currentObject) {
	return std::static_pointer_cast<EnemyBase>(currentObject);
}
