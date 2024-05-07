#pragma once
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_pixels.h>

#include "vector2.h"

class SDL_Surface;
class SDL_Texture;

class TextSprite {
public:
	void Init(const char* fontType, int fontSize, const char* text, SDL_Color color);

	void ChangeText(const char* text, SDL_Color color);

	void SetTargetPosition(Vector2<float> position);

	void ClearText();

	void Render();
	void RenderCentered();

private:
	TTF_Font* _font;
	SDL_Surface * _textSurface;
	SDL_Texture* _textTexture;

	float _posX;
	float _posY;
};

