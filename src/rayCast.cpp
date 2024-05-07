#include "rayCast.h"

#include "debugDrawer.h"
#include "gameEngine.h"

RayPoint RayCast::RayCastToAABB(AABB boxCollider, Ray ray) {
	_rayDirection = ray.startPosition + ray.direction;
	RayPoint rayPoint = ClosestPoint(ray, { 
		FindPoint(boxCollider.min, { boxCollider.min.x, boxCollider.max.y }, ray, _rayDirection),
		FindPoint({ boxCollider.min.x, boxCollider.max.y }, boxCollider.max, ray, _rayDirection),
		FindPoint(boxCollider.min, { boxCollider.max.x, boxCollider.min.y }, ray, _rayDirection),
		FindPoint({ boxCollider.max.x, boxCollider.min.y }, boxCollider.max, ray, _rayDirection) 
	});

	if (ray.length < Vector2<float>::distanceBetweenVectors(ray.startPosition, rayPoint.position)) {
		return RayPoint();
	}

	return rayPoint;
}

RayPoint RayCast::FindPoint(Vector2<float> wallStart, Vector2<float> wallEnd, Ray ray, Vector2<float> rayDir) {
	_denominator = (wallStart.x - wallEnd.x) * (ray.startPosition.y - rayDir.y) -
		(wallStart.y - wallEnd.y) * (ray.startPosition.x - rayDir.x);
	if (_denominator == 0) {
		return RayPoint();
	}
	_t = ((wallStart.x - ray.startPosition.x) * (ray.startPosition.y - rayDir.y) -
		(wallStart.y - ray.startPosition.y) * (ray.startPosition.x - rayDir.x)) / _denominator;
	_u = -((wallStart.x - wallEnd.x) * (wallStart.y - ray.startPosition.y) -
		(wallStart.y - wallEnd.y) * (wallStart.x - ray.startPosition.x)) / _denominator;
	if (_t > 0 && _t < 1 && _u > 0) {
		_rayPoint.position.x = wallStart.x + _t * (wallEnd.x - wallStart.x);
		_rayPoint.position.y = wallStart.y + _t * (wallEnd.y - wallStart.y);
		_rayPoint.pointHit = true;
		_rayPointNormalA = { wallStart.y - wallEnd.y, wallStart.x - wallEnd.x };
		_rayPointNormalA.normalize();
		_rayPointNormalB = { -_rayPointNormalA.x, -_rayPointNormalA.y };
		if (Vector2<float>::distanceBetweenVectors(ray.startPosition, _rayPoint.position + _rayPointNormalA) <
			Vector2<float>::distanceBetweenVectors(ray.startPosition, _rayPoint.position + _rayPointNormalB)) {
			_rayPoint.normal = _rayPointNormalA;
		} else {
			_rayPoint.normal = _rayPointNormalB;
		}

		return _rayPoint;
	}
	_rayPoint.pointHit = false;
	_rayPoint.normal = { 0.f, 0.f };
	_rayPoint.position = { 0.f, 0.f };
	return _rayPoint;
}

RayPoint RayCast::ClosestPoint(Ray ray, std::array<RayPoint, 4> points) {
	_distanceToWall[0] = Vector2<float>::distanceBetweenVectors(ray.startPosition, points[0].position);
	_distanceToWall[1] = Vector2<float>::distanceBetweenVectors(ray.startPosition, points[1].position);
	_distanceToWall[2] = Vector2<float>::distanceBetweenVectors(ray.startPosition, points[2].position);
	_distanceToWall[3] = Vector2<float>::distanceBetweenVectors(ray.startPosition, points[3].position);
	
	if (_distanceToWall[0] < _distanceToWall[1] && _distanceToWall[0] < _distanceToWall[2] && _distanceToWall[0] < _distanceToWall[3]) {
		if (points[0].pointHit) {
			return points[0];
		}
	}
	if (_distanceToWall[1] < _distanceToWall[0] && _distanceToWall[1] < _distanceToWall[2] && _distanceToWall[1] < _distanceToWall[3]) {
		if (points[1].pointHit) {
			return points[1];
		}
	}
	if (_distanceToWall[2] < _distanceToWall[0] && _distanceToWall[2] < _distanceToWall[1] && _distanceToWall[2] < _distanceToWall[3]) {
		if (points[2].pointHit) {
			return points[2];
		}
	}
	if (_distanceToWall[3] < _distanceToWall[0] && _distanceToWall[3] < _distanceToWall[1] && _distanceToWall[3] < _distanceToWall[2]) {
		if (points[3].pointHit) {
			return points[3];
		}
	}
	return RayPoint();
}
