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
	for (auto& projectile : _activeObjects) {
		_currentProjectile = CastAsProjectile(projectile.second);
		_currentProjectile->Update();
		_currentProjectile->QueryObjects();
		if (CheckCollision(_currentProjectile->GetProjectileType(), _currentProjectile->GetObjectID())) {
			_removeObjects.emplace_back(_currentProjectile);
			continue;
		}	
	}
	while (_removeObjects.size() > 0) {
		RemoveObject(_removeObjects.back()->GetObjectID());
		_removeObjects.pop_back();
	}
	_currentProjectile = nullptr;
}

void ProjectileManager::InsertObjectsQuadtree() {
	for (auto& object : _activeObjects) {
		if (!object.second->GetCollider()->GetIsActive()) {
			continue;
		}
		projectileQuadTree->Insert(object.second, object.second->GetCollider());
	}
}

void ProjectileManager::CreateNewProjectile(const ProjectileType& projectileType) {
	_projectilePools[projectileType]->PoolObject(std::make_shared<Projectile>(projectileType, _spritePaths[projectileType]));
}

std::shared_ptr<Projectile> ProjectileManager::SpawnProjectile(std::shared_ptr<ObjectBase> owner, const ProjectileType& projectileType,
	const float& orientation, const Vector2<float>& direction, const Vector2<float>& position, const unsigned int& damage, const float& speed) {
	if (_projectilePools[projectileType]->IsEmpty()) {
		CreateNewProjectile(projectileType);
	}
	_currentProjectile = _projectilePools[projectileType]->SpawnObject();
	_currentProjectile->ActivateProjectile(owner, orientation, direction, position, damage, speed);
	_activeObjects.insert(std::make_pair(_currentProjectile->GetObjectID(), _currentProjectile));
	return _currentProjectile;
}

bool ProjectileManager::CheckCollision(const ProjectileType& projectileType, const unsigned int& objectID) {
	_projectileHit = false;
	if (universalFunctions->OutsideBorderX(_activeObjects[objectID]->GetPosition().x, _activeObjects[objectID]->GetSprite()->GetHeight()) ||
		universalFunctions->OutsideBorderY(_activeObjects[objectID]->GetPosition().y, _activeObjects[objectID]->GetSprite()->GetHeight())) {
		_projectileHit = true;
		return _projectileHit;
	}
	_objectsHit = _activeObjects[objectID]->GetQueriedObjects();
	for (unsigned int i = 0; i < _objectsHit.size(); i++) {
		if (_objectsHit[i]->GetObjectType() == _activeObjects[objectID]->GetObjectType() ||
			_objectsHit[i]->GetObjectType() == CastAsProjectile(_activeObjects[objectID])->GetOwner()->GetObjectType()) {
			continue;
		}
		_objectsHit[i]->TakeDamage(CastAsProjectile(_activeObjects[objectID])->GetDamage());
		_projectileHit = true;
	}
	return _projectileHit;
}

void ProjectileManager::RemoveAllObjects() {
	for (auto& projectile : _activeObjects) {
		_currentProjectile = CastAsProjectile(projectile.second);
		_currentProjectile->DeactivateObject();
		_projectilePools[_currentProjectile->GetProjectileType()]->PoolObject(_currentProjectile);
	}
	_activeObjects.clear();
	_currentProjectile = nullptr;
}

void ProjectileManager::RemoveObject(const unsigned int& objectID) {
	if (_activeObjects.empty()) {
		return;
	}	
	_currentProjectile = CastAsProjectile(_activeObjects[objectID]);
	_currentProjectile->DeactivateObject();
	_projectilePools[_currentProjectile->GetProjectileType()]->PoolObject(_currentProjectile);
	_activeObjects.erase(objectID);
	_currentProjectile = nullptr;
}

std::shared_ptr<Projectile> ProjectileManager::CastAsProjectile(std::shared_ptr<ObjectBase> object) {
	return std::static_pointer_cast<Projectile>(object);
}