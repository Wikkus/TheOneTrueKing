#include "projectileManager.h"

#include "enemyManager.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "objectPool.h"
#include "playerCharacter.h"
#include "quadTree.h"
#include "searchSortAlgorithms.h"

ProjectileManager::ProjectileManager() {
	_spritePaths[ProjectileType::Energyblast] = _energyBlastSprite;
	_spritePaths[ProjectileType::EnemyFireball] = _enemyProjectileSprite;
	_spritePaths[ProjectileType::PlayerFireball] = _playerProjectileSprite;

	for (unsigned int i = 0; i < (unsigned int)ProjectileType::Count; i++) {
		_projectilePools[(ProjectileType)i] = std::make_shared<ObjectPool<std::shared_ptr<Projectile>>>(_projectileAmountLimit);
	}
}

ProjectileManager::~ProjectileManager() {}

void ProjectileManager::Init() {
	for (unsigned int i = 0; i < _projectileAmountLimit; i++) {
		CreateNewProjectile(ProjectileType::PlayerFireball);
		CreateNewProjectile(ProjectileType::EnemyFireball);
		if (i % 10 == 0) {
			CreateNewProjectile(ProjectileType::Energyblast);
		}
	}
}

void ProjectileManager::Update() {
	for (unsigned int i = 0; i < _activeObjects.size(); i++) {
		_currentProjectile = CastAsProjectile(_activeObjects[i]);
		_currentProjectile->Update();
		_currentProjectile->QueryObjects();

		if (universalFunctions->OutsideBorderX(_currentProjectile->GetPosition().x, 0.f) ||
			universalFunctions->OutsideBorderY(_currentProjectile->GetPosition().y, 0.f)) {
			RemoveProjectile(_currentProjectile->GetProjectileType(), _currentProjectile->GetObjectID());
			continue;
		}
		if (CheckCollision(_currentProjectile->GetProjectileType(), i)) {
			continue;
		}
	}
	_currentProjectile = nullptr;
}

void ProjectileManager::Render() {
	for (unsigned int i = 0; i < _activeObjects.size(); i++) {
		_activeObjects[i]->Render();
	}
}

void ProjectileManager::CreateNewProjectile(ProjectileType projectileType) {
	_projectilePools[projectileType]->PoolObject(std::make_shared<Projectile>(lastObjectID, projectileType, _spritePaths[projectileType]));
}

void ProjectileManager::SpawnProjectile(std::shared_ptr<ObjectBase> owner, ProjectileType projectileType, float orientation,
	Vector2<float> direction, Vector2<float> position, unsigned int damage, float speed) {
	if (_projectilePools[projectileType]->IsEmpty()) {
		CreateNewProjectile(projectileType);
	}
	_activeObjects.emplace_back(_projectilePools[projectileType]->SpawnObject());
	_objectIDs.emplace_back(_activeObjects.back()->GetObjectID());
	CastAsProjectile(_activeObjects.back())->ActivateProjectile(owner, orientation, direction, position, damage, speed);
}

bool ProjectileManager::CheckCollision(ProjectileType projectileType, unsigned int projectileIndex) {
	_objectsHit = _activeObjects[projectileIndex]->GetQueriedObjects();
	for (unsigned int i = 0; i < _objectsHit.size(); i++) {
		if (_objectsHit[i]->GetObjectType() == ObjectType::Projectile ||
			_objectsHit[i]->GetObjectType() == CastAsProjectile(_activeObjects[projectileIndex])->GetOwner()->GetObjectType()) {
			continue;
		}
		_objectsHit[i]->TakeDamage(CastAsProjectile(_activeObjects[projectileIndex])->GetDamage());
		RemoveProjectile(projectileType, _activeObjects[projectileIndex]->GetObjectID());
		return true;
	}
	return false;
}

void ProjectileManager::RemoveAllProjectiles() {
	while (_activeObjects.size() > 0) {
		_currentProjectile = CastAsProjectile(_activeObjects.back());
		_currentProjectile->DeactivateObject();
		_projectilePools[_currentProjectile->GetProjectileType()]->PoolObject(_currentProjectile);
		_activeObjects.pop_back();
	}
	_objectIDs.clear();
}

void ProjectileManager::RemoveProjectile(ProjectileType projectileType, unsigned int projectileID) {
	if (_activeObjects.empty()) {
		return;
	}
	QuickSort(_objectIDs, 0, _objectIDs.size() - 1);
	_latestProjectileIndex = quickSort->BinarySearch(_objectIDs, 0, _objectIDs.size() - 1, projectileID);
	if (_latestProjectileIndex < 0) {
		return;
	}
	_currentProjectile = CastAsProjectile(_activeObjects[_latestProjectileIndex]);
	_currentProjectile->DeactivateObject();
	_projectilePools[projectileType]->PoolObject(_currentProjectile);
	std::swap(_activeObjects[_latestProjectileIndex], _activeObjects.back());
	std::swap(_objectIDs[_latestProjectileIndex], _objectIDs.back());
	_activeObjects.pop_back();
	_objectIDs.pop_back();
	_latestProjectileIndex = -1;
	_currentProjectile = nullptr;
}

void ProjectileManager::Reset() {
	RemoveAllProjectiles();
}

std::shared_ptr<Projectile> ProjectileManager::CastAsProjectile(std::shared_ptr<ObjectBase> object) {
	return std::static_pointer_cast<Projectile>(object);
}