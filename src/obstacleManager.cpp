#include "obstacleManager.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "objectPool.h"
#include "obstacleWall.h"
#include "quadTree.h"

ObstacleManager::ObstacleManager() {
	_objectAmountLimit = 200;
	_obstaclePool = std::make_shared<ObjectPool<std::shared_ptr<Obstacle>>>(_objectAmountLimit);
}

void ObstacleManager::Init() {
	for (unsigned int i = 0; i < _objectAmountLimit; i++) {
		CreateNewObstacle();
	}
}

void ObstacleManager::CreateNewObstacle() {
	_obstaclePool->PoolObject(std::make_shared<Obstacle>());
}

void ObstacleManager::SpawnObstacle(const Vector2<float>& position, const float& width, const float& height, const SDL_Color& color) {
	if (_obstaclePool->IsEmpty()) {
		CreateNewObstacle();
	}
	_currentObstacle = _obstaclePool->SpawnObject();
	_currentObstacle->ActivateObstacle(position, width, height, color);
	_activeObjects.insert(std::make_pair(_currentObstacle->GetObjectID(), _currentObstacle));
}

void ObstacleManager::InsertObjectsQuadtree() {
	for (auto& object : _activeObjects) {
		if (!object.second->GetCollider()->GetIsActive()) {
			continue;
		}
		obstacleQuadTree->Insert(object.second, object.second->GetCollider());
	}
}

void ObstacleManager::RemoveAllObjects() {
	for (auto& obstacle : _activeObjects) {
		_currentObstacle = CastAsObstacle(obstacle.second);
		_currentObstacle->DeactivateObject();
		_obstaclePool->PoolObject(_currentObstacle);
	}
	_activeObjects.clear();
	_currentObstacle = nullptr;
}

void ObstacleManager::RemoveObject(const unsigned int& objectID) {
	if (_activeObjects.empty()) {
		return;
	}
	_currentObstacle = CastAsObstacle(_activeObjects[objectID]);
	_currentObstacle->DeactivateObject();
	_obstaclePool->PoolObject(_currentObstacle);
	_activeObjects.erase(objectID);
	_currentObstacle = nullptr;
}

std::shared_ptr<Obstacle> ObstacleManager::CastAsObstacle(std::shared_ptr<ObjectBase> objectBase) {
	return std::static_pointer_cast<Obstacle>(objectBase);
}
