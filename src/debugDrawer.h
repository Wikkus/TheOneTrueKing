#pragma once
#include "SDL2/SDL_pixels.h"
#include "vector2.h"

#include <array>
#include <vector>

struct DebugBox {
	Vector2<float> min = Vector2<float>(0.f, 0.f);
	Vector2<float> max = Vector2<float>(0.f, 0.f);
	Vector2<float> position = Vector2<float>(0.f, 0.f);
	SDL_Color color = { 255, 255, 255, 255 };
};
struct DebugCircle {
	Vector2<float> position = Vector2<float>(0.f, 0.f);
	float radius = 0.f;
	SDL_Color color = { 255, 255, 255, 255 };
};
struct DebugLine {
	Vector2<float> startPosition = Vector2<float>(0.f, 0.f);
	Vector2<float> endPosition = Vector2<float>(0, 0.f);
	SDL_Color color = { 255, 255, 255, 255 };
};


class DebugDrawer {
public:
	DebugDrawer() {}
	~DebugDrawer() {}

	void AddDebugBox(Vector2<float> position, Vector2<float> min, Vector2<float> max, SDL_Color color);
	void AddDebugRectangle(Vector2<float> position, Vector2<float> min, Vector2<float> max, SDL_Color color);
	void AddDebugCircle(Vector2<float> position, float radius, SDL_Color color);
	void AddDebugCross(Vector2<float> position, float length, SDL_Color color);
	void AddDebugLine(Vector2<float> startPosition, Vector2<float> endPosition, SDL_Color color);

	void DrawBoxes();
	void DrawRectangles();
	void DrawCircles();
	void DrawLines();

private:
	std::vector<DebugBox> _debugBoxes;
	std::vector<DebugBox> _debugRectangles;
	std::vector<DebugLine> _debugLines;
	std::vector<DebugCircle> _debugCircles;

};

