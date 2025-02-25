#pragma once
#include "collision.h"
#include "managerBase.h"
#include "vector2.h"

#include <SDL2/SDL.h>
#include <vector>

class Wall;

class ObstacleManager : public ManagerBase {
public:
	ObstacleManager(){}
	~ObstacleManager(){}

	void CreateWall(Vector2<float> position, float width, float height, SDL_Color color);

	void Update() override;
	void Render() override;

	std::shared_ptr<Wall> CastAsWall(std::shared_ptr<ObjectBase> objectBase);

private:
	std::shared_ptr<Wall> _currentWall = nullptr;

};