#include "obstacleManager.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "obstacleWall.h"

ObstacleManager::ObstacleManager() {
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
	_currentObstacle->ActivateObstacle(position, width, height, color);
	_activeObjects.insert(std::make_pair(_currentObstacle->GetObjectID(), _currentObstacle));
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
