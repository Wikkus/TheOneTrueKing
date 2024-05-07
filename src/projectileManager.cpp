#include "projectileManager.h"

#include "dataStructuresAndMethods.h"
#include "enemyManager.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "objectPool.h"
#include "playerCharacter.h"
#include "quadTree.h"

ProjectileManager::ProjectileManager() {
	_numberOfProjectileTypes = (unsigned int)ProjectileType::Count;
	for (unsigned int i = 0; i < _numberOfProjectileTypes; i++) {
		_projectilePools[(ProjectileType)i] = std::make_shared<ObjectPool<std::shared_ptr<Projectile>>>(_projectileAmountLimit);
	}
}

ProjectileManager::~ProjectileManager() {}

void ProjectileManager::Init() {
	for (unsigned int i = 0; i < _projectileAmountLimit; i++) {
		if (i % 5 == 0) {
			CreateNewProjectile(ProjectileType::PlayerProjectile, _playerProjectileSprite, 0.f, 30, {0.f, 0.f}, {-10000.f, -10000.f});
		}
		CreateNewProjectile(ProjectileType::EnemyProjectile, _enemyProjectileSprite, 0.f, 1, {0.f, 0.f}, {-10000.f, -10000.f});
	}
}

void ProjectileManager::Update() {
	for (unsigned int i = 0; i < _activeProjectiles.size(); i++) {
		_activeProjectiles[i]->Update();
		if (CheckCollision(_activeProjectiles[i]->GetProjectileType(), i)) {
			continue;
		}
		if (OutOfBorderX(_activeProjectiles[i]->GetPosition().x) ||
			OutOfBorderY(_activeProjectiles[i]->GetPosition().y)) {
			RemoveProjectile(_activeProjectiles[i]->GetProjectileType(), _activeProjectiles[i]->GetObjectID());
		}
	}
}

void ProjectileManager::Render() {
	for (unsigned int i = 0; i < _activeProjectiles.size(); i++) {
		_activeProjectiles[i]->Render();
	}
}

void ProjectileManager::CreateNewProjectile(ProjectileType projectileType, const char* spritePath, float orientation, unsigned int projectileDamage, Vector2<float> direction, Vector2<float> position) {
	_projectilePools[projectileType]->PoolObject(std::make_shared<Projectile>(projectileType, spritePath, projectileDamage, _lastProjectileID));
	_lastProjectileID++;
}

void ProjectileManager::SpawnProjectile(ProjectileType projectileType, const char* spritePath, float orientation, unsigned int projectileDamage, Vector2<float> direction, Vector2<float> position) {
	if (_projectilePools[projectileType]->IsEmpty()) {
		CreateNewProjectile(projectileType, spritePath, orientation, projectileDamage, direction, position);
		_activeProjectiles.emplace_back(_projectilePools[projectileType]->SpawnObject());
		_activeProjectiles.back()->ActivateProjectile(orientation, direction, position);
	} else {
		_activeProjectiles.emplace_back(_projectilePools[projectileType]->SpawnObject());
		_activeProjectiles.back()->ActivateProjectile(orientation,direction, position);
	}	
}

bool ProjectileManager::CheckCollision(ProjectileType projectileType, unsigned int projectileIndex) {
	if (projectileType == ProjectileType::EnemyProjectile) {
		if (IsInDistance(playerCharacter->GetPosition(), _activeProjectiles[projectileIndex]->GetPosition(),
			_activeProjectiles[projectileIndex]->GetCollider().radius)) {
			playerCharacter->TakeDamage(_activeProjectiles[projectileIndex]->GetProjectileDamage());
			RemoveProjectile(projectileType, _activeProjectiles[projectileIndex]->GetObjectID());
			return true;
		}
		return false;
	}
	_objectsHit = objectBaseQuadTree->Query(_activeProjectiles[projectileIndex]->GetCollider());
	for (unsigned int i = 0; i < _objectsHit.size(); i++) {
		if (!_objectsHit[i]) {
			continue;
		}
		if (projectileType == ProjectileType::PlayerProjectile) {
			if (_objectsHit[i]->GetObjectType() == ObjectType::Enemy) {
				_enemyHit = std::static_pointer_cast<EnemyBase>(_objectsHit[i]);	
				//Returns true if the enemy dies
				if (_enemyHit->TakeDamage(_activeProjectiles[projectileIndex]->GetProjectileDamage())) {
					enemyManager->RemoveEnemy(_enemyHit->GetEnemyType(), _enemyHit->GetObjectID());
				}
				RemoveProjectile(projectileType, _activeProjectiles[projectileIndex]->GetObjectID());
				return true;
			}
		} /*else if (projectileType == ProjectileType::EnemyProjectile) {
			if (_objectsHit[i]->GetObjectType() == ObjectType::Player) {
				playerCharacter->TakeDamage(_activeProjectiles[projectileIndex]->GetProjectileDamage());
				RemoveProjectile(projectileType, _activeProjectiles[projectileIndex]->GetObjectID());
				return true;
			}
		}*/
	}

	return false;
}

const char* ProjectileManager::GetEnemyProjectileSprite() const {
	return _enemyProjectileSprite;
}

const char* ProjectileManager::GetPlayerProjectileSprite() const {
	return _enemyProjectileSprite;
}

void ProjectileManager::RemoveAllProjectiles() {
	while (_activeProjectiles.size() > 0) {
		_activeProjectiles.back()->DeactivateProjectile();
		_projectilePools[_activeProjectiles.back()->GetProjectileType()]->PoolObject(_activeProjectiles.back());
		_activeProjectiles.pop_back();
	}
}

void ProjectileManager::RemoveProjectile(ProjectileType projectileType, unsigned int projectileID) {
	if (_activeProjectiles.empty()) {
		return;
	} else if (_activeProjectiles.size() > 1) {
		QuickSort(0, _activeProjectiles.size() - 1);
		_latestProjectileIndex = BinarySearch(0, _activeProjectiles.size() - 1, projectileID);
		if (_latestProjectileIndex >= 0) {
			_activeProjectiles[_latestProjectileIndex]->DeactivateProjectile();
			_projectilePools[projectileType]->PoolObject(_activeProjectiles[_latestProjectileIndex]);
			std::swap(_activeProjectiles[_latestProjectileIndex], _activeProjectiles.back());
		}
		_latestProjectileIndex = -1;
	}
	_activeProjectiles.pop_back();
}

void ProjectileManager::UpdateQuadTree() {
	for (unsigned int i = 0; i < _activeProjectiles.size(); i++) {
		objectBaseQuadTree->Insert(_activeProjectiles[i], _activeProjectiles[i]->GetCollider());
	}
}

int ProjectileManager::BinarySearch(int low, int high, int targetID) {
	while (low <= high) {
		int mid = low + (high - low) / 2;

		if (_activeProjectiles[mid]->GetObjectID() == targetID) {
			return mid;
		}

		if (_activeProjectiles[mid]->GetObjectID() < targetID) {
			low = mid + 1;
		} else {
			high = mid - 1;
		}
	}
	return -1;
}

int ProjectileManager::Partition(int start, int end) {
	unsigned int pivot = _activeProjectiles[start]->GetObjectID();

	int count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (_activeProjectiles[i]->GetObjectID() <= pivot) {
			count++;
		}
	}

	int pivotIndex = start + count;
	std::swap(_activeProjectiles[pivotIndex], _activeProjectiles[start]);

	int i = start, k = end;

	while (i < pivotIndex && k > pivotIndex) {

		while (_activeProjectiles[i]->GetObjectID() <= pivot) {
			i++;
		}

		while (_activeProjectiles[k]->GetObjectID() > pivot) {
			k--;
		}

		if (i < pivotIndex && k > pivotIndex) {
			std::swap(_activeProjectiles[i++], _activeProjectiles[k--]);
		}
	}
	return pivotIndex;
}

void ProjectileManager::QuickSort(int start, int end) {
	if (start >= end) {
		return;
	}
	int p = Partition(start, end);

	QuickSort(start, p - 1);
	QuickSort(p + 1, end);
}
