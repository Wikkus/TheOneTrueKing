#pragma once
#include "collision.h"
#include "vector2.h"

#include <array>

class Wall {
public:
	Wall() {}
	~Wall() {}

	void Init(Vector2<float> position, float width, float height, unsigned int wallID, std::array<int, 4> color);
	void Update();
	void Render();

	const Vector2<float> GetPosition() const;
	const AABB GetCollider() const;

private:
	Vector2<float> _position = { 0.f, 0.f };
	float _height = 0.f;
	float _width = 0.f;
	unsigned int _wallID = 0;

	AABB _boxCollider;

	std::array<int, 4> _color{ 255, 255, 255, 255 };


};