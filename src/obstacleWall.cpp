#include "obstacleWall.h"

#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"

Wall::Wall(unsigned int objectID) : ObjectBase(objectID, ObjectType::Obstacle) {
	_boxCollider = std::make_shared<AABB>();
}

void Wall::Activate(Vector2<float> position, float width, float height, unsigned int wallID, SDL_Color color) {
	_width = width;
	_height = height;
	_position = position;
	_boxCollider->SetPosition(position);
	_boxCollider->SetHeight(height);
	_boxCollider->SetWidth(width);
	_color = color;
	Init();
}

void Wall::Init() {}

void Wall::Update() {}

void Wall::Render() {
	SDL_Rect rect = {
		_boxCollider->GetMin().x,
		_boxCollider->GetMin().y,
		_boxCollider->GetMax().x - _boxCollider->GetMin().x,
		_boxCollider->GetMax().y - _boxCollider->GetMin().y,
	};
	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void Wall::RenderText() {}

const std::shared_ptr<Collider> Wall::GetCollider() const {
	return _boxCollider;
}