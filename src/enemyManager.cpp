#include "enemyManager.h"

#include "bossBoar.h"
#include "enemyBase.h"
#include "enemyBoar.h"
#include "enemyHuman.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "obstacleManager.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "searchSortAlgorithms.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "timerHandler.h"
#include "weaponComponent.h"
#include "weaponManager.h"

EnemyManager::EnemyManager() {
	//Creates an unordered map with objectpool of the different enemy types
	_numberOfEnemyTypes = (unsigned int)EnemyType::Count;
	for (unsigned int i = 0; i < _numberOfEnemyTypes; i++) {
		_enemyPools[(EnemyType)i] = std::make_shared<ObjectPool<std::shared_ptr<EnemyBase>>>(_enemyAmountLimit);
	}
	_spawnPositions = {
		Vector2<float>(windowWidth, windowHeight * 0.5f),
		Vector2<float>(windowWidth * 0.5f, windowHeight),
		Vector2<float>(0.f, windowHeight * 0.5f),
		Vector2<float>(windowWidth * 0.5f, 0.f)
	};
}

EnemyManager::~EnemyManager() {}

void EnemyManager::Init() {
	_spawnTimer = timerHandler->SpawnTimer(3.f, false, false);
	//Create all enemies in the object pool at the start of the project
	for (unsigned i = 0; i < _enemyAmountLimit / _numberOfEnemyTypes; i++) {
		for (unsigned int k = 0; k < _numberOfEnemyTypes; k++) {
			CreateNewEnemy(EnemyType(k));
		}
	}
}

void EnemyManager::Update() {
	if (_spawnTimer->GetIsFinished()) {
		_spawnEnemy = true;
	}
	for (auto& objectBase : _activeObjects) {
		objectBase.second->Update();
		objectBase.second->QueryObjects();
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
void EnemyManager::UpdateTestState() {
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

const std::vector<std::shared_ptr<FormationHandler>> EnemyManager::GetFormationManagers() const {
	return _formationManagers;
}

//Creates a specific enemy based on the enemyType enum
void EnemyManager::CreateNewEnemy(const EnemyType& enemyType) {
	switch (enemyType) {
	case EnemyType::Boar:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyBoar>());
		break;

	case EnemyType::Human:
		_enemyPools[enemyType]->PoolObject(std::make_shared<EnemyHuman>());
		break;
	
	default:
		break;
	}
}

void EnemyManager::BossSpawner() {
	_currentEnemy = std::make_shared<BoarBoss>();
	_currentEnemy->Init();
	_activeObjects.insert(std::make_pair(_currentEnemy->GetObjectID(), _currentEnemy));
	_currentEnemy = nullptr;
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
		SpawnFormation(_spawnCountPerRow, FormationType::VShape);
	}
	_spawnCountPerRow[0] += 2;
	_spawnTimer->SetTimer(false, false);
}

void EnemyManager::SpawnFormation(const std::array<unsigned int, 2>& spawnCountPerRow, const FormationType& formationType) {
	_formationManagers.emplace_back(std::make_shared<FormationHandler>(formationType, spawnCountPerRow, _latestAnchorPoint, false));	
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
		
		default:
			_currentWeaponType = WeaponType::Staff;
			break;
		}
		_currentSpawnAmount = _formationManagers.back()->GetFormationPattern()->GetSlotsPerType()[_currentAttackType];
		for (unsigned int k = 0; k < _currentSpawnAmount; k++) {
			enemyManager->SpawnEnemy(EnemyType::Human, 0, { 0.f, 0.f }, _latestAnchorPoint->position, _currentWeaponType);
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
			_weaponPicked = decideWeapon(randomEngine);
			switch (_weaponPicked) {
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

void EnemyManager::TestEnemySpawner() {
	_spawnCountPerRow = { 15, 3 };	
	_latestAnchorPoint = std::make_shared<AnchorPoint>();
	_latestAnchorPoint->position = _spawnPositions[0];
	_latestAnchorPoint->targetPosition = { windowWidth * 0.5f, windowHeight * 0.5f };
	_latestAnchorPoint->direction = _latestAnchorPoint->targetPosition - _latestAnchorPoint->position;
	_latestAnchorPoint->orientation = universalFunctions->VectorAsOrientation(_latestAnchorPoint->direction);
	SpawnFormation(_spawnCountPerRow, FormationType::VShape);

}

//Spawn a specific enemy from the object pool. If the pool is empty, create a new enemy of that type
std::shared_ptr<EnemyBase> EnemyManager::SpawnEnemy(const EnemyType& enemyType, const float& orientation,
	const Vector2<float>& direction, const Vector2<float>& position, const WeaponType& weaponType) {
	if (_enemyPools[enemyType]->IsEmpty()) {
		CreateNewEnemy(enemyType);
	}
	//Then add the enemy to the active enemies map which is called in Update
	_currentEnemy = _enemyPools[enemyType]->SpawnObject();			
	_currentEnemy->ActivateEnemy(orientation, direction, position, weaponType);
	_activeObjects.insert(std::make_pair(_currentEnemy->GetObjectID(), _currentEnemy));
	return _currentEnemy;
}

void EnemyManager::RemoveAllObjects() {
	for (auto& enemy : _activeObjects) {
		_currentEnemy = CastAsEnemy(enemy.second);
		_currentEnemy->DeactivateObject();
		_enemyPools[_currentEnemy->GetEnemyType()]->PoolObject(_currentEnemy);
	}
	_activeObjects.clear();
	_currentEnemy = nullptr;
}

void EnemyManager::RemoveObject(const unsigned int& objectID) {
	if (_activeObjects.empty()) {
		return;
	}
	_currentEnemy = CastAsEnemy(_activeObjects[objectID]);

	if (_formationManagers.size() > 0) {
		_formationManagers[_currentEnemy->GetFormationIndex()]->RemoveCharacter(_currentEnemy);
	}
	//Deactivate the enemy by setting its position to a far away place
	_currentEnemy->DeactivateObject();
	
	//Adds the enemy to the object pool and place it at the back of the vector
	_enemyPools[_currentEnemy->GetEnemyType()]->PoolObject(_currentEnemy);

	//Removes the enemy from active enemies
	_activeObjects.erase(objectID);

	if (_activeObjects.empty()) {
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
	RemoveAllObjects();
	_spawnTimer->ResetTimer();
}

const unsigned int EnemyManager::GetWaveNumber() const {
	return _waveNumber;
}

std::shared_ptr<EnemyBase> EnemyManager::CastAsEnemy(std::shared_ptr<ObjectBase> currentObject) {
	return std::static_pointer_cast<EnemyBase>(currentObject);
}
