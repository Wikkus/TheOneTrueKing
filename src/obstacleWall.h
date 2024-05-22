#pragma once
#include "collision.h"
#include "objectBase.h"
#include "vector2.h"

#include <array>

class Wall : public ObjectBase {
public:
	Wall(unsigned int objectID);
	~Wall() {}

	void Init() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

	void Activate(Vector2<float> position, float width, float height, unsigned int wallID, SDL_Color color);

	const unsigned int GetObjectID() const override;
	const std::shared_ptr<Collider> GetCollider() const override;
	const ObjectType GetObjectType() const override;

	const Vector2<float> GetPosition() const override;
	const std::shared_ptr<Sprite> GetSprite() const override;
	const Vector2<float> GetVelocity() const override;

private:
	float _height = 0.f;
	float _width = 0.f;

	std::shared_ptr<AABB> _boxCollider = nullptr;
	SDL_Color _color = { 255, 255, 255, 255 };
};