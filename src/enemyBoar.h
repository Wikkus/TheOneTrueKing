#pragma once
#include "collision.h"
#include "enemyBase.h"
#include "sprite.h"
#include "vector2.h"

#include <memory>

class EnemyBoar : public EnemyBase {
public:
	EnemyBoar();
	~EnemyBoar();

	void Init() override;
	void Update() override;

private:
	const char* _boarSprite = "res/sprites/MadBoar.png";
};

