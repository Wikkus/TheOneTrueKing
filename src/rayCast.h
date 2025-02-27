#pragma once
#include "collision.h"
#include "vector2.h"

#include <array>

struct RayPoint {
	Vector2<float> position = { 0.f, 0.f };
	Vector2<float> normal = { 0.f, 0.f };
	bool pointHit = false;	
};

class RayCast {
public:
	RayCast(){}
	~RayCast(){}

	RayPoint RayCastToAABB(std::shared_ptr<AABB> boxCollider, const Ray& ray);

	RayPoint FindPoint(const Vector2<float>& wallStart, const Vector2<float>& wallEnd, const Ray& ray, const Vector2<float>& rayDir);
	RayPoint ClosestPoint(const Ray& ray, const std::array<RayPoint, 4>& points);


private:
	std::array<float, 4> _distanceToWall = {0.f, 0.f, 0.f, 0.f };
	RayPoint _rayCastPoint;

	float _denominator = 0.f;
	float _t = 0.f;
	float _u = 0.f;

	RayPoint _rayPoint;

	Vector2<float> _rayDirection = { 0.f, 0.f };
	Vector2<float> _rayPointNormalA = { 0.f, 0.f };
	Vector2<float> _rayPointNormalB = { 0.f, 0.f };

};