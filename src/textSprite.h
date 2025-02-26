#pragma once
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_pixels.h>

#include "vector2.h"

class SDL_Surface;
class SDL_Texture;

class TextSprite {
public:
	void Init(const char* fontType, int fontSize, const char* text, const SDL_Color& color);

	void ChangeText(const char* text, const SDL_Color& color);

	void SetPosition(const Vector2<float>& position);

	void ClearText();

	void Render();
	void RenderCentered();

private:
	TTF_Font* _font;
	SDL_Surface * _textSurface;
	SDL_Texture* _textTexture;
	
	SDL_FRect _textDist;

	Vector2<float> _position;
};

