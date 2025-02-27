#pragma once
#include <SDL2/SDL_image.h>

#include "vector2.h"

class Sprite {
public:
	Sprite() {}
	~Sprite() {}

	void Load(const char* path);

	virtual void Render(const int& spriteIndex, const Vector2<float>& position);
	virtual void RenderCentered(const int& spriteIndex, const Vector2<float>& position);
	virtual void RenderWithOrientation(const int& spriteIndex, const Vector2<float>& position, const float& orientation);

	const float GetWidth() const;
	const float GetHeight() const;

protected:
	SDL_FPoint _center;
	SDL_FRect _rect;
	SDL_Rect _rectCopy;
	SDL_FRect _rectDest;
	SDL_Texture* _texture = nullptr;

	float _widthF = 0.f;
	float _heightF = 0.f;
	int _widthI = 0;
	int _heightI = 0;
};

class SpriteSheet : public Sprite {
public:
	SpriteSheet() {}
	~SpriteSheet() {}

	void Render(const int& spriteIndex, const Vector2<float>& position) override;
	void RenderCentered(const int& spriteIndex, const Vector2<float>& position) override;
	void RenderWithOrientation(const int& spriteIndex, const Vector2<float>& position, const float& orientation) override;
};