#include "textSprite.h"
#include "gameEngine.h"

#include <SDL2/SDL.h>

void TextSprite::Init(const char* fontType, int fontSize, const char* text, SDL_Color color) {
	_font = TTF_OpenFont(fontType, fontSize);
	_textSurface = TTF_RenderText_Solid(_font, text, color);
	_textTexture = SDL_CreateTextureFromSurface(renderer, _textSurface);
}

void TextSprite::ChangeText(const char* text, SDL_Color color) {
	_textSurface = TTF_RenderText_Solid(_font, text, color);
	_textTexture = SDL_CreateTextureFromSurface(renderer, _textSurface);
}

void TextSprite::SetTargetPosition(Vector2<float> position) {
	_posX = position.x;
	_posY = position.y;
}

void TextSprite::ClearText() {
	SDL_FreeSurface(_textSurface);
	SDL_DestroyTexture(_textTexture);
}

void TextSprite::Render() {
	SDL_Rect textDst = { _posX, _posY, _textSurface->w, _textSurface->h };
	SDL_RenderCopy(renderer, _textTexture, nullptr, &textDst);
}

void TextSprite::RenderCentered() {
	SDL_Rect textDist = { ((int)_posX - _textSurface->w / 2), _posY - _textSurface->h / 2, _textSurface->w, _textSurface->h };
	SDL_RenderCopy(renderer, _textTexture, nullptr, &textDist);
}
