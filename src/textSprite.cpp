#include "textSprite.h"
#include "gameEngine.h"

#include <SDL2/SDL.h>

void TextSprite::Init(const char* fontType, int fontSize, const char* text, const SDL_Color& color) {
	_font = TTF_OpenFont(fontType, fontSize);
	_textSurface = TTF_RenderText_Solid(_font, text, color);
	_textTexture = SDL_CreateTextureFromSurface(renderer, _textSurface);
}

void TextSprite::ChangeText(const char* text, const SDL_Color& color) {
	_textSurface = TTF_RenderText_Solid(_font, text, color);
	_textTexture = SDL_CreateTextureFromSurface(renderer, _textSurface);
}

void TextSprite::SetPosition(const Vector2<float>& position) {
	_position = position;
}

void TextSprite::ClearText() {
	SDL_FreeSurface(_textSurface);
	SDL_DestroyTexture(_textTexture);
}

void TextSprite::Render() {	
	_textDist = { _position.x, _position.y, (float)_textSurface->w, (float)_textSurface->h };
	SDL_RenderCopyF(renderer, _textTexture, nullptr, &_textDist);
}

void TextSprite::RenderCentered() {
	_textDist = { (_position.x - _textSurface->w / 2), _position.y - _textSurface->h / 2, (float)_textSurface->w, (float)_textSurface->h };
	SDL_RenderCopyF(renderer, _textTexture, nullptr, &_textDist);
}
