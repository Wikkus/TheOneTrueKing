#include "obstacleManager.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "obstacleWall.h"

void ObstacleManager::CreateWall(Vector2<float> position, float width, float height, SDL_Color color) {
	std::shared_ptr<Wall> wall = std::make_shared<Wall>();
	wall->Activate(position, width, height, color);
	_activeObjects.insert(std::make_pair(wall->GetObjectID(), wall));
}

void ObstacleManager::Update() {
	for (auto& objectBase : _activeObjects) {
		_currentWall = CastAsWall(objectBase.second);
		_currentWall->Update();
		_currentWall->QueryObjects();		
	}
}

void ObstacleManager::Render() {
	for (auto& objectBase : _activeObjects) {
		_currentWall = CastAsWall(objectBase.second);
		_currentWall->Render();
	}
}

std::shared_ptr<Wall> ObstacleManager::CastAsWall(std::shared_ptr<ObjectBase> objectBase) {
	return std::static_pointer_cast<Wall>(objectBase);
}
