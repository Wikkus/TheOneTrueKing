#include "obstacleManager.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "obstacleWall.h"

void ObstacleManager::CreateWall(Vector2<float> position, float width, float height, std::array<int, 4> color) {
	std::shared_ptr<Wall> wall = std::make_shared<Wall>();
	wall->Init(position, width, height, _walls.size(), color);
	_walls.emplace_back(wall);
}

void ObstacleManager::UpdateObstacles() {
	for (int i = 0; i < _walls.size(); i++) {
		_walls[i]->Update();
	}
}

void ObstacleManager::RenderObstacles() {
	for (int i = 0; i < _walls.size(); i++) {
		_walls[i]->Render();
	}
}

std::vector<std::shared_ptr<Wall>> const ObstacleManager::GetWalls() {
	return _walls;
}
