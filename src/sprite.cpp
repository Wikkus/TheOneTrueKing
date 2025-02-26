#include "sprite.h"
#include "gameEngine.h"

Sprite::Sprite() {}

void Sprite::Load(const char* path) {
	_texture = IMG_LoadTexture(renderer, path);
	SDL_QueryTexture(_texture, NULL, NULL, &_widthI, &_heightI);
	_widthF = _widthI;
	_heightF = _heightI;
}

void Sprite::Render(const int& spriteIndex, const Vector2<float>& position) {
	_rect = { position.x, position.y, _widthF, _heightF };	
	SDL_RenderCopyF(renderer, _texture, NULL, &_rect);
}

void Sprite::RenderCentered(const int& spriteIndex, const Vector2<float>& position) {
	_rect = { position.x - _widthF / 2, position.y - _heightF / 2, _widthF, _heightF };
	SDL_RenderCopyF(renderer, _texture, nullptr, &_rect);
}

void Sprite::RenderWithOrientation(const int& spriteIndex, const Vector2<float>& position, const float& orientation) {
	_rectCopy = { 0, 0, _widthI, _heightI };
	_rectDest = { position.x - _widthF / 2, position.y - _heightF / 2, _widthF, _heightF };
	_center = { _widthF / 2, _heightF / 2 };
	SDL_RenderCopyExF(renderer, _texture, &_rectCopy, &_rectDest, orientation * 180 / PI, &_center, SDL_FLIP_NONE);
}

const float Sprite::GetWidth() const {
	return _widthF;
}

const float Sprite::GetHeight() const {
	return _heightF;
}

SpriteSheet::SpriteSheet() {}

void SpriteSheet::Render(const int& spriteIndex, const Vector2<float>& position) {
	_rectCopy = { spriteIndex * _widthI, 0, _widthI, _heightI };
	_rectDest = { position.x, position.y, _widthF, _heightF };
	SDL_RenderCopyF(renderer, _texture, &_rectCopy, &_rectDest);
}

void SpriteSheet::RenderCentered(const int& spriteIndex, const Vector2<float>& position) {
	_rectCopy = { spriteIndex * _widthI, 0, _widthI, _heightI };
	_rectDest = { position.x - _widthF / 2, position.y - _heightF / 2, _widthF, _heightF };
	SDL_RenderCopyF(renderer, _texture, &_rectCopy, &_rectDest);
}

void SpriteSheet::RenderWithOrientation(const int& spriteIndex, const Vector2<float>& position, const float& orientation) {
	_rectCopy = { spriteIndex * _widthI, 0, _widthI, _heightI };
	_rectDest = { position.x - _widthF / 2, position.y - _heightF / 2, _widthF, _heightF };
	_center = { _widthF / 2, _heightF / 2 };
	SDL_RenderCopyExF(renderer, _texture, &_rectCopy, &_rectDest,
		orientation * 180 / PI, &_center, SDL_FLIP_NONE);
}