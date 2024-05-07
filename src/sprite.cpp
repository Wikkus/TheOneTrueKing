#include "sprite.h"
#include "gameEngine.h"

void Sprite::Load(const char* path) {
	texture = IMG_LoadTexture(renderer, path);
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
}

void Sprite::Render(Vector2<float> position) {
	SDL_Rect rect = { (int)position.x, (int)position.y, w, h };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::RenderCentered(Vector2<float> position) {
	SDL_Rect rect = { (int)position.x - w / 2, (int)position.y - h / 2, w, h };
	SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Sprite::RenderWithOrientation(Vector2<float> position, float orientation) {
	SDL_Rect source = { 0, 0, w, h };
	SDL_Rect destination = { (int)position.x - w / 2, (int)position.y - h / 2, w, h };
	SDL_Point center = { w / 2, h / 2 };
	SDL_RenderCopyEx(renderer, texture, &source, &destination, orientation * 180 / PI, &center, SDL_FLIP_NONE);
}


