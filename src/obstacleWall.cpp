#include "obstacleWall.h"

#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"

Obstacle::Obstacle() : ObjectBase(ObjectType::Obstacle) {
	_boxCollider = std::make_shared<AABB>();
}

void Obstacle::ActivateObstacle(const Vector2<float>& position, const float& width, const float& height, const SDL_Color& color) {
	_position = position;
	_width = width;
	_height = height;
	_boxCollider->SetPosition(position);
	_boxCollider->SetWidth(_width);
	_boxCollider->SetHeight(_height);
	_color = color;
	Init();
}

void Obstacle::Init() {}

void Obstacle::Update() {}

void Obstacle::Render() {
	_rect.x = _boxCollider->GetMin().x;
	_rect.y = _boxCollider->GetMin().y;
	_rect.w = _boxCollider->GetMax().x - _boxCollider->GetMin().x;
	_rect.h = _boxCollider->GetMax().y - _boxCollider->GetMin().y;

	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
	SDL_RenderFillRect(renderer, &_rect);
}

void Obstacle::RenderText() {}

const std::shared_ptr<Collider> Obstacle::GetCollider() const {
	return _boxCollider;
}
