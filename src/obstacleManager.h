#pragma once
#include "collision.h"
#include "managerBase.h"
#include "objectPool.h"
#include "vector2.h"

#include <SDL2/SDL.h>
#include <vector>

class Obstacle;

class ObstacleManager : public ManagerBase {
public:
	ObstacleManager();
	~ObstacleManager(){}

	void Init() override;

	void CreateNewObstacle();
	void SpawnObstacle(const Vector2<float>& position,
		const float& width, const float& height, const SDL_Color& color);

	void RemoveAllObjects() override;
	void RemoveObject(const unsigned int& objectID) override;

private:
	std::shared_ptr<ObjectPool<std::shared_ptr<Obstacle>>> _obstaclePool;
	std::shared_ptr<Obstacle> CastAsObstacle(std::shared_ptr<ObjectBase> objectBase);
	std::shared_ptr<Obstacle> _currentObstacle = nullptr;

};