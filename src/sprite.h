#pragma once
#include <SDL2/SDL_image.h>

#include "vector2.h"

struct Sprite {
	void Load(const char* path);
	void Render(Vector2<float> position);
	void RenderCentered(Vector2<float> position);
	void RenderWithOrientation(Vector2<float> position, float orientation);

	SDL_Texture* texture;
	int w;
	int h;
};
