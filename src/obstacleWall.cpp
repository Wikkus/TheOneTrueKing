#include "obstacleWall.h"

#include "collision.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "quadTree.h"

Obstacle::Obstacle() : ObjectBase(ObjectType::Obstacle) {
	_boxCollider = std::make_shared<AABB>();
	_collider = _boxCollider;
}

void Obstacle::ActivateObstacle(const Vector2<float>& position, const float& width, const float& height, const SDL_Color& color) {
	_position = position;
	_width = width;
	_height = height;
	_boxCollider->SetWidth(_width);
	_boxCollider->SetHeight(_height);
	_boxCollider->SetPosition(position);
	_color = color;
	Init();
}

void Obstacle::Init() {}

void Obstacle::Update() {}

void Obstacle::Render() {
	_rect.x = _boxCollider->GetPosition().x - (_width * 0.5f);
	_rect.y = _boxCollider->GetPosition().y - (_height * 0.5f);
	_rect.w = _width;
	_rect.h = _height;

	//debugDrawer->AddDebugRectangle(_boxCollider->GetPosition(), _boxCollider->GetMin(), _boxCollider->GetMax(), { 0, 255, 0, 255}, false);

	SDL_SetRenderDrawColor(renderer, _color.r, _color.g, _color.b, _color.a);
	SDL_RenderFillRectF(renderer, &_rect);
}

void Obstacle::RenderText() {}

const std::shared_ptr<Collider> Obstacle::GetCollider() const {
	return _boxCollider;
}

void Obstacle::QueryObjects() {
	_queriedObjects = objectBaseQuadTree->Query(_collider);
	_queriedProjectiles = obstacleQuadTree->Query(_collider);
	_queriedObjects.insert(_queriedObjects.end(), _queriedProjectiles.begin(), _queriedProjectiles.end());
}
