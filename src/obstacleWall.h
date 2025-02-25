#pragma once
#include "collision.h"
#include "objectBase.h"
#include "vector2.h"

#include <array>

class Wall : public ObjectBase {
public:
	Wall();
	~Wall() {}

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	void Activate(Vector2<float> position, float width, float height, SDL_Color color);

	const std::shared_ptr<Collider> GetCollider() const override;

private:
	float _height = 0.f;
	float _width = 0.f;

	std::shared_ptr<AABB> _boxCollider = nullptr;
	SDL_Color _color = { 255, 255, 255, 255 };
};