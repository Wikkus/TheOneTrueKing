#pragma once
#include <SDL2/SDL.h>
#include "SDL2/SDL_pixels.h"
#include "vector2.h"

#include <array>
#include <vector>

struct DebugRectangle {
	Vector2<float> min = { 0.f, 0.f };
	Vector2<float> max = { 0.f, 0.f };
	Vector2<float> position = { -10000.f, -10000.f };
	SDL_Color color = { 255, 255, 255, 255 };
	bool fill = false;
};
struct DebugCircle {
	Vector2<float> position = { -10000.f, -10000.f };
	float radius = 0.f;
	SDL_Color color = { 255, 255, 255, 255 };
};
struct DebugLine {
	Vector2<float> startPosition = { -10000.f, -10000.f };
	Vector2<float> endPosition = { -10000.f, -10000.f };
	SDL_Color color = { 255, 255, 255, 255 };
};


class DebugDrawer {
public:
	DebugDrawer(){}
	~DebugDrawer() {}

	void AddDebugRectangle(const Vector2<float>& position, const Vector2<float>& min, const Vector2<float>& max, const SDL_Color& color, const bool& fill);
	void AddDebugCircle(const Vector2<float>& position, const float& radius, const SDL_Color& color);
	void AddDebugCross(const Vector2<float>& position, const float& length, const SDL_Color& color);
	void AddDebugLine(const Vector2<float>& startPosition, const Vector2<float>& endPosition, const SDL_Color& color);

	void DrawRectangles();
	void DrawCircles();
	void DrawLines();

private:
	std::vector<DebugRectangle> _debugRectangles;
	std::vector<DebugLine> _debugLines;
	std::vector<DebugCircle> _debugCircles;

	DebugRectangle _debugRectangle;
	DebugCircle _debugCircle;
	DebugLine _debugLine;

	SDL_Rect _rect{ -10000, -10000, 0, 0 };

	int _resolution = 24;

	float _angle = 0.f;
	float _nextAngle = 0.f;
	float _step = 0.f;

	Vector2<float> _positionA = { -10000.f, -10000.f };
	Vector2<float> _positionB = { -10000.f, -10000.f };
};

