#pragma once
#include "collision.h"
#include "vector2.h"

#include <vector>

class Wall;

class ObstacleManager {
public:
	ObstacleManager(){}
	~ObstacleManager(){}

	void CreateWall(Vector2<float> position, float width, float height, std::array<int, 4> color);

	void UpdateObstacles();
	void RenderObstacles();

	const std::vector<std::shared_ptr<Wall>> const GetWalls();

private:
	std::vector<std::shared_ptr<Wall>> _walls;

};