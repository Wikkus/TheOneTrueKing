#include "obstacleWall.h"

#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"

void Wall::Init(Vector2<float> position, float width, float height, unsigned int wallID, std::array<int, 4> color) {
	_width = width;
	_height = height;
	_position = position;
	_boxCollider = AABB::makeFromPositionSize(_position, _height, _width);
	_color = color;
	_wallID = wallID;
}

void Wall::Update() {}

void Wall::Render() {
	SDL_Rect rect = {
		_boxCollider.min.x,
		_boxCollider.min.y,
		_boxCollider.max.x - _boxCollider.min.x,
		_boxCollider.max.y - _boxCollider.min.y,
	};
	SDL_SetRenderDrawColor(renderer, _color[0], _color[1], _color[2], _color[3]);
	SDL_RenderFillRect(renderer, &rect);
}

const Vector2<float> Wall::GetPosition() const {
	return _position;
}

const AABB Wall::GetCollider() const {
	return _boxCollider;
}
