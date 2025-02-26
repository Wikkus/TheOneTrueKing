#include "debugDrawer.h"
#include "gameEngine.h"

void DebugDrawer::AddDebugRectangle(const Vector2<float>& position, const Vector2<float>& min, const Vector2<float>& max, const SDL_Color& color, const bool& fill) {
	_debugRectangle.color = color;
	_debugRectangle.min = min;
	_debugRectangle.max = max;
	_debugRectangle.position = position;
	_debugRectangle.fill = fill;
	_debugRectangles.emplace_back(_debugRectangle);
}

void DebugDrawer::AddDebugCircle(const Vector2<float>& position, const float& radius, const SDL_Color& color) {
	_debugCircle.color = color;
	_debugCircle.position = position;
	_debugCircle.radius = radius;
	_debugCircles.emplace_back(_debugCircle);
}

void DebugDrawer::AddDebugCross(const Vector2<float>& position, const float& length, const SDL_Color& color) {
	_debugLine.color = color;
	//Horizontal line
	_debugLine.startPosition = { position.x - (length * 0.5f), position.y };
	_debugLine.endPosition = { position.x + (length * 0.5f), position.y };
	_debugLines.emplace_back(_debugLine);

	//Vertical line
	_debugLine.startPosition = { position.x, position.y - (length * 0.5f) };
	_debugLine.endPosition = { position.x, position.y + (length * 0.5f) };
	_debugLines.emplace_back(_debugLine);
}

void DebugDrawer::AddDebugLine(const Vector2<float>& startPosition, const Vector2<float>& endPosition, const SDL_Color& color) {
	_debugLine.color = color;
	_debugLine.startPosition = startPosition;
	_debugLine.endPosition = endPosition;
	_debugLines.emplace_back(_debugLine);
}

void DebugDrawer::DrawRectangles() {
	for (int i = 0; i < _debugRectangles.size(); i++) {
		SDL_SetRenderDrawColor(renderer, _debugRectangles[i].color.r, _debugRectangles[i].color.g, _debugRectangles[i].color.b, _debugRectangles[i].color.a);
		_rect.x = _debugRectangles[i].position.x - ((_debugRectangles[i].max.x - _debugRectangles[i].min.x) * 0.5f);
		_rect.y = _debugRectangles[i].position.y - ((_debugRectangles[i].max.y - _debugRectangles[i].min.y) * 0.5f);

		_rect.w = _debugRectangles[i].max.x - _debugRectangles[i].min.x;
		_rect.h = _debugRectangles[i].max.y - _debugRectangles[i].min.y;
		if (_debugRectangles[i].fill) {
			SDL_RenderFillRect(renderer, &_rect);

		} else {
			SDL_RenderDrawRect(renderer, &_rect);
		}
	}
	_debugRectangles.clear();
}

void DebugDrawer::DrawCircles() {
	for (int i = 0; i < _debugCircles.size(); i++) {
		SDL_SetRenderDrawColor(renderer, _debugCircles[i].color.r, _debugCircles[i].color.g, _debugCircles[i].color.b, _debugCircles[i].color.a);

		_resolution = 24;
		_step = (2 * PI) / _resolution;

		for (int k = 0; k < _resolution; k++) {
			_angle = _step * k;
			_positionA.x = cos(_angle);
			_positionA.y = sin(_angle);

			_nextAngle = _step * (k + 1);
			_positionB.x = cos(_nextAngle);
			_positionB.y = sin(_nextAngle);
			SDL_RenderDrawLine(
				renderer,
				_positionA.x * _debugCircles[i].radius + _debugCircles[i].position.x,
				_positionA.y * _debugCircles[i].radius + _debugCircles[i].position.y,
				_positionB.x * _debugCircles[i].radius + _debugCircles[i].position.x,
				_positionB.y * _debugCircles[i].radius + _debugCircles[i].position.y);
		}
	}
	_debugCircles.clear();
}

void DebugDrawer::DrawLines() {
	for (int i = 0; i < _debugLines.size(); i++) {
	SDL_SetRenderDrawColor(renderer, _debugLines[i].color.r, _debugLines[i].color.g, _debugLines[i].color.b, _debugLines[i].color.a);
		SDL_RenderDrawLineF(renderer, _debugLines[i].startPosition.x, _debugLines[i].startPosition.y, 
			_debugLines[i].endPosition.x, _debugLines[i].endPosition.y);
	}
	_debugLines.clear();
}
