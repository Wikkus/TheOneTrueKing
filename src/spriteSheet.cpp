#include "spriteSheet.h"
#include "gameEngine.h"

void SpriteSheet::Load(const char* path, int width, int height) {
	texture = IMG_LoadTexture(renderer, path);
	w = width;
	h = height;
}

void SpriteSheet::Render(int spriteIndex, Vector2<float> position) {
	SDL_Rect source = { spriteIndex * w, 0, w, h };
	SDL_Rect destination = { (int)position.x, (int)position.y, w, h };
	SDL_RenderCopy(renderer, texture, &source, &destination);
}

void SpriteSheet::RenderCentered(int spriteIndex, Vector2<float> position) {
	SDL_Rect source = { spriteIndex * w, 0, w, h };
	SDL_Rect destination = { (int)position.x - w / 2, (int)position.y - h / 2, w, h };
	SDL_RenderCopy(renderer, texture, &source, &destination);
}

void SpriteSheet::RenderWithOrientation(int spriteIndex, Vector2<float> position, float orientation) {
	SDL_Rect source = { spriteIndex * w, 0, w, h };
	SDL_Rect destination = { (int)position.x - w / 2, (int)position.y - h / 2, w, h };
	SDL_Point center = { w / 2, h / 2 };

	SDL_RenderCopyEx(renderer, texture, &source, &destination, 
		orientation * 180 / PI, &center, SDL_FLIP_NONE);
}
