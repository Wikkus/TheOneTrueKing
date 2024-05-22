#include "enemyManager.h"

#include "enemyBase.h"
#include "enemyBoar.h"
#include "enemyHuman.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "timerManager.h"
#include "weaponComponent.h"

EnemyManager::EnemyManager() {
	//Creates an unordered map with objectpool of the different enemy types
	_enemyPools[EnemyType::Boar] = std::make_shared<ObjectPool<std::shared_ptr<EnemyBase>>>(_enemyAmountLimit);
	_enemyPools[EnemyType::Human] = std::make_shared<ObjectPool<std::shared_ptr<EnemyBase>>>(_enemyAmountLimit);	

	_numberOfEnemyTypes = (unsigned int)EnemyType::Count;
}

EnemyManager::~EnemyManager() {
	RemoveAllEnemies();
}

void EnemyManager::Init() {
	_spawnTimer = timerManager->CreateTimer(3.f);
	int enemyTypes = (int)EnemyType::Count;
	//Create all enemies in the object pool at the start of the project
	for (unsigned i = 0; i < _enemyAmountLimit / enemyTypes; i++) {
		for (unsigned int k = 0; k < _numberOfEnemyTypes; k++) {
			CreateNewEnemy(EnemyType(k), 0.f, {0.f, 0.f}, {-10000.f, -10000.f});
		}
	}
}

void EnemyManager::Update() {}

void EnemyManager::UpdateSurvival() {
	if (_spawnTimer->GetTimerFinished() && _activeEnemies.size() < _enemyAmountLimit) {
		SurvivalEnemySpawner();
	}	
	for (unsigned i = 0; i < _activeEnemies.size(); i++) {
		_activeEnemies[i]->SetTargetPosition(playerCharacter->GetPosition());
		_activeEnemies[i]->Update();
	}
}

void EnemyManager::UpdateFormation() {
	if (_spawnTimer->GetTimerFinished() && _activeEnemies.size() < _enemyAmountLimit) {
		TacticalEnemySpawner();
	}	
	for (unsigned int i = 0; i < _formationManagers.size(); i++) {
		_formationManagers[i]->UpdateSlots();
	}
	for (unsigned i = 0; i < _activeEnemies.size(); i++) {
		_activeEnemies[i]->Update();
	}
}

void EnemyManager::Render() {
	for (unsigned i = 0; i < _activeEnemies.size(); i++) {
		_activeEnemies[i]->Render();
	}
}

std::vector<std::shared_ptr<EnemyBase>> EnemyManager::GetActiveEnemies() {
	return _activeEnemies;
}

//Creates a specific enemy based on the enemyType enum
void EnemyManager::CreateNewEnemy(EnemyType enemyType, float orientation, Vector2<float> direction, Vector2<float> position) {
	switch (enemyType) {
	case EnemyType::Boar:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyBoar>(objectID, enemyType));
		break;

	case EnemyType::Human:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyHuman>(objectID, enemyType));
		break;
	
	default:
		break;
	}	
	objectID++;
}

void EnemyManager::TacticalEnemySpawner() {
	AnchorPoint anchorPoint;
	anchorPoint.position = { windowWidth, windowHeight * 0.5f };
	anchorPoint.orientation = 0.f;
	
	unsigned int spawnNumber = (_enemiesInFormation + (_waveNumber * 3));

	
	SpawnTactical(spawnNumber, anchorPoint, FormationType::VShape, SlotAttackType::Count, WeaponType::Count);

	//anchorPoint.position.x += 32.f;
	//SpawnTactical(spawnNumber, anchorPoint, FormationType::VShape, SlotAttackType::MeleeAttacker, WeaponType::Sword);
	//
	//anchorPoint.position.x += 32.f;	
	//SpawnTactical(spawnNumber, anchorPoint, FormationType::VShape, SlotAttackType::MagicAttacker, WeaponType::Staff);

	_spawnTimer->DeactivateTimer();
}

void EnemyManager::SpawnTactical(unsigned int spawnNumber, AnchorPoint anchorPoint, 
	FormationType formationType, SlotAttackType sloatAttackType, WeaponType weaponType) {
	_formationManagers.emplace_back(std::make_shared<FormationManager>(formationType,
		spawnNumber, anchorPoint, true, sloatAttackType));
	int enemiesSpawned = 0;
	while (enemiesSpawned < spawnNumber) {
		enemyManager->SpawnEnemy(EnemyType::Human, anchorPoint.orientation, Vector2<float>(0.f, 0.f), anchorPoint.position, weaponType);
		_activeEnemies.back()->SetFormationIndex(_formationManagers.size() - 1);
		_formationManagers.back()->AddCharacter(_activeEnemies.back());
		enemiesSpawned++;
	}
}

void EnemyManager::SurvivalEnemySpawner() {
	std::uniform_int_distribution dist{ 0, 3 };
	unsigned int amountEnemies = (_spawnNumberOfEnemies + 4) + _waveNumber;
	for (unsigned int i = 0; i < amountEnemies; i++) {
		Vector2<float> spawnPosition = { 0.f, 0.f };
		if (i < amountEnemies * 0.5f) {
			float distX = 0.f;
			std::uniform_real_distribution<float> distY{ 0.f, windowHeight };
			int temp = dist(randomEngine);
			if (temp == 0) {
				distX = 0;
			} else {
				distX = windowWidth;
			}
			spawnPosition = { distX, distY(randomEngine) };

		} else {
			std::uniform_real_distribution<float> distX{ 0.f, windowWidth };
			float distY = 0.f;
			int temp = dist(randomEngine);
			if (temp == 0) {
				distY = 0;
			} else {
				distY = windowHeight;
			}
			spawnPosition = { distX(randomEngine), distY };
		}
		Vector2<float> direction = (playerCharacter->GetPosition() - spawnPosition).normalized();
		if (i % 3 == 0) {
			enemyManager->SpawnEnemy(EnemyType::Boar, VectorAsOrientation(direction), direction, spawnPosition, WeaponType::Count);
		} else {
			enemyManager->SpawnEnemy(EnemyType::Human, VectorAsOrientation(direction), direction, spawnPosition, WeaponType::Count);
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
	_activeEnemies.emplace_back(_enemyPools[enemyType]->SpawnObject());
	_activeEnemies.back()->ActivateEnemy(orientation, direction, position, weaponType);
}

void EnemyManager::RemoveAllEnemies() {
	while (_activeEnemies.size() > 0) {
		_activeEnemies.back()->DeactivateEnemy();
		_enemyPools[_activeEnemies.back()->GetEnemyType()]->PoolObject(_activeEnemies.back());
		_activeEnemies.pop_back();
	}
}
//Using Quicksort and Binary search to locate a specific enemy
void EnemyManager::RemoveEnemy(EnemyType enemyType, unsigned int objectID) {
	if (_activeEnemies.empty()) {
		return;
	} else if (_activeEnemies.size() >= 1) {
		//Sorts the active enemies vector based on their ID from lowest to highest
		QuickSort(0, _activeEnemies.size() - 1);
		//Search through the sorted vector to find the enemy with a specific ID
		_latestEnemyIndex = BinarySearch(0, _activeEnemies.size() - 1, objectID);
		if (_latestEnemyIndex >= 0) {
			if (_formationManagers.size() > 0) {
				_formationManagers[_activeEnemies[_latestEnemyIndex]->GetFormationIndex()]->RemoveCharacter(_activeEnemies[_latestEnemyIndex]);				
				if (_formationManagers[_activeEnemies[_latestEnemyIndex]->GetFormationIndex()]->GetNumberOfSlots() <= 0) {
					_formationManagers.erase(_formationManagers.begin() + _activeEnemies[_latestEnemyIndex]->GetFormationIndex());
				}
			}
			//Deactivate the enemy by setting its position to a far away place
			_activeEnemies[_latestEnemyIndex]->DeactivateEnemy();
			//Adds the enemy to the object pool and place it at the back of the vector
			_enemyPools[enemyType]->PoolObject(_activeEnemies[_latestEnemyIndex]);
			std::swap(_activeEnemies[_latestEnemyIndex], _activeEnemies.back());		
		}
	}
	//Removes the enemy from active enemies
	_activeEnemies.pop_back();
	if (gameStateHandler->GetGameMode() == GameMode::Formation) {
		if (_activeEnemies.size() <= 0) {
			_waveNumber++;
			_spawnTimer->ResetTimer();
		}
	}
}

void EnemyManager::Reset() {
	_waveNumber = 0;
	RemoveAllEnemies();
	_spawnTimer->ResetTimer();
}

void EnemyManager::TakeDamage(unsigned int enemyIndex, unsigned int damageAmount) {
	if(_activeEnemies[enemyIndex]->TakeDamage(damageAmount)) {
		RemoveEnemy(_activeEnemies[enemyIndex]->GetEnemyType(), enemyIndex);
	}
}

void EnemyManager::UpdateQuadTree() {
	for (unsigned i = 0; i < _activeEnemies.size(); i++) {
		std::shared_ptr<Circle> temp = std::static_pointer_cast<Circle>(_activeEnemies[i]->GetCollider());
		objectBaseQuadTree->Insert(_activeEnemies[i], *temp);
		enemyBaseQuadTree->Insert(_activeEnemies[i], *temp);
	}
}
const unsigned int EnemyManager::GetWaveNumber() const {
	return _waveNumber;
}
//Binary search based on ID
int EnemyManager::BinarySearch(int low, int high, int objectID) {
	while (low <= high) {
		int mid = low + (high - low) / 2;

		if (_activeEnemies[mid]->GetObjectID() == objectID) {
			return mid;
		}

		if (_activeEnemies[mid]->GetObjectID() < objectID) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}
	return -1;
}
//Quicksort based on ID
int EnemyManager::Partition(int start, int end) {
	int pivot = _activeEnemies[start]->GetObjectID();

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (_activeEnemies[i]->GetObjectID() <= pivot) {
			count++;
		}
	}

	int pivotIndex = start + count;
	std::swap(_activeEnemies[pivotIndex], _activeEnemies[start]);

	int i = start, k = end;

	while (i < pivotIndex && k > pivotIndex) {

		while (_activeEnemies[i]->GetObjectID() <= pivot) {
			i++;
		}

		while (_activeEnemies[k]->GetObjectID() > pivot) {
			k--;
		}

		if (i < pivotIndex && k > pivotIndex) {
			std::swap(_activeEnemies[i++], _activeEnemies[k--]);
		}
	}
	return pivotIndex;
}

void EnemyManager::QuickSort(int start, int end) {
	if (start >= end) {
		return;
	}
	int p = Partition(start, end);

	QuickSort(start, p - 1);
	QuickSort(p + 1, end);
}
