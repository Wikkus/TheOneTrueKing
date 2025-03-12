#pragma once
#include "collision.h"
#include "objectBase.h"
#include "vector2.h"

#include <array>


class Obstacle : public ObjectBase {
public:
	Obstacle();
	~Obstacle() {}

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	void ActivateObstacle(const Vector2<float>& position, const float& width, const float& height, const SDL_Color& color);
	
	const std::shared_ptr<Collider> GetCollider() const override;

	void QueryObjects() override;

private:	
	SDL_FRect _rect;

	float _height = 0.f;
	float _width = 0.f;

	std::shared_ptr<AABB> _boxCollider = nullptr;
	SDL_Color _color = { 255, 255, 255, 255 };
};