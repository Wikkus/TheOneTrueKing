#pragma once
#include <SDL2/SDL_image.h>

#include "vector2.h"

struct SpriteSheet {
	void Load(const char* path, int width, int height);
	void Render(int spriteIndex, Vector2<float> position);
	void RenderCentered(int spriteIndex, Vector2<float> position);
	void RenderWithOrientation(int spriteIndex, Vector2<float> position, float orientation);

	SDL_Texture* texture;
	int w;
	int h;
};

