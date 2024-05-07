#include "debugDrawer.h"
#include "gameEngine.h"

#include <SDL2/SDL.h>

void DebugDrawer::AddDebugBox(Vector2<float> position, Vector2<float> min, Vector2<float> max, SDL_Color color) {
	DebugBox debugBox;
	debugBox.min = min;
	debugBox.max = max;
	debugBox.position = position;
	debugBox.color = color;
	_debugBoxes.emplace_back(debugBox);
}

void DebugDrawer::AddDebugRectangle(Vector2<float> position, Vector2<float> min, Vector2<float> max, SDL_Color color) {
	DebugBox debugBox;
	debugBox.min = min;
	debugBox.max = max;
	debugBox.position = position;
	debugBox.color = color;
	_debugRectangles.emplace_back(debugBox);
}

void DebugDrawer::AddDebugCircle(Vector2<float> position, float radius, SDL_Color color) {
	DebugCircle debugCircle;
	debugCircle.position = position;
	debugCircle.radius = radius;
	debugCircle.color = color;
	_debugCircles.emplace_back(debugCircle);
}

void DebugDrawer::AddDebugCross(Vector2<float> position, float length, SDL_Color color) {
	DebugLine debugLineHorizontal;
	DebugLine debugLineVertical;

	debugLineHorizontal.startPosition = Vector2<float>(position.x - (length * 0.5f), position.y);
	debugLineHorizontal.endPosition = Vector2<float>(position.x + (length * 0.5f), position.y);
	debugLineHorizontal.color = color;

	debugLineVertical.startPosition = Vector2<float>(position.x, position.y - (length * 0.5f));
	debugLineVertical.endPosition = Vector2<float>(position.x, position.y + (length * 0.5f));
	debugLineVertical.color = color;

	_debugLines.emplace_back(debugLineHorizontal);
	_debugLines.emplace_back(debugLineVertical);
}

void DebugDrawer::AddDebugLine(Vector2<float> startPosition, Vector2<float> endPosition, SDL_Color color) {
	DebugLine debugLine;
	debugLine.startPosition = startPosition;
	debugLine.endPosition = endPosition;
	debugLine.color = color;
	_debugLines.emplace_back(debugLine);
}

void DebugDrawer::DrawBoxes() {
	for (int i = 0; i < _debugBoxes.size(); i++) {
		SDL_SetRenderDrawColor(renderer, _debugBoxes[i].color.r, _debugBoxes[i].color.g, _debugBoxes[i].color.b, _debugBoxes[i].color.a);
		SDL_Rect rect = {
			_debugBoxes[i].position.x - ((_debugBoxes[i].max.x - _debugBoxes[i].min.x) * 0.5f),
			_debugBoxes[i].position.y - ((_debugBoxes[i].max.y - _debugBoxes[i].min.y) * 0.5f),
			_debugBoxes[i].max.x - _debugBoxes[i].min.x,
			_debugBoxes[i].max.y - _debugBoxes[i].min.y
		};
		SDL_RenderFillRect(renderer, &rect);
	}
	_debugBoxes.clear();
}

void DebugDrawer::DrawRectangles() {
	for (int i = 0; i < _debugRectangles.size(); i++) {
		SDL_SetRenderDrawColor(renderer, _debugRectangles[i].color.r, _debugRectangles[i].color.g, _debugRectangles[i].color.b, _debugRectangles[i].color.a);
		SDL_Rect rect = {
			_debugRectangles[i].position.x - ((_debugRectangles[i].max.x - _debugRectangles[i].min.x) * 0.5f),
			_debugRectangles[i].position.y - ((_debugRectangles[i].max.y - _debugRectangles[i].min.y) * 0.5f),
			_debugRectangles[i].max.x - _debugRectangles[i].min.x,
			_debugRectangles[i].max.y - _debugRectangles[i].min.y			
		};
		SDL_RenderDrawRect(renderer, &rect);
	}
	_debugRectangles.clear();
}

void DebugDrawer::DrawCircles() {
	for (int i = 0; i < _debugCircles.size(); i++) {
		SDL_SetRenderDrawColor(renderer, _debugRectangles[i].color.r, _debugRectangles[i].color.g, _debugRectangles[i].color.b, _debugRectangles[i].color.a);

		int resolution = 24;
		float step = (2 * PI) / resolution;

		for (int k = 0; k < resolution; k++) {

			float angle = step * k;
			float x1 = cos(angle);
			float y1 = sin(angle);

			float nextAngle = step * (k + 1);
			float x2 = cos(nextAngle);
			float y2 = sin(nextAngle);

			SDL_RenderDrawLine(
				renderer,
				x1 * _debugCircles[i].radius + _debugCircles[i].position.x,
				y1 * _debugCircles[i].radius + _debugCircles[i].position.y,
				x2 * _debugCircles[i].radius + _debugCircles[i].position.x,
				y2 * _debugCircles[i].radius + _debugCircles[i].position.y);

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
