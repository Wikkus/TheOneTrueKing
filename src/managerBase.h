#pragma once
#include "vector2.h"

#include <memory>
#include <unordered_map>
#include <vector>

class ObjectBase;

struct BehaviorData {
	float targetOrientation = 0.f;

	float timeToTarget = 0.1f;

	//align data
	float angularSlowDownRadius = 0.f;
	float angularTargetRadius = 0.f;
	float maxAngularAcceleration = 0.f;
	float maxRotation = 0.f;

	//seek data
	float maxLinearAcceleration = 25.f;
	float maxSpeed = 50.f;
	float linearSlowDownRadius = 50.f;
	float linearTargetRadius = 10.f;

	//pursue data
	float maxPrediction = 0.f;
	Vector2<float> targetsVelocity = Vector2<float>{ 0.f, 0.f };

	//collision avoidance data
	float characterRadius = 0.f;

	//Obstacle avoidance data
	float avoidDistance = 0.f;
	float lookAhead = 0.f;

	//separation data
	float separationThreshold = 0.f;
	float attractionThreshold = 0.f;
	float decayCoefficient = 0.f;

	//wander data
	float wanderOffset = 0.f;
	float wanderRadius = 0.f;
	float wanderRate = 0.f;
};

class ManagerBase {
public:
	ManagerBase();
	~ManagerBase() {}

	virtual void Init() {}
	virtual void Update();

	virtual void Render();
	virtual void RenderText();
	
	virtual void InsertObjectsQuadtree();
	virtual void RemoveAllObjects() {}
	virtual void RemoveObject(const unsigned int& objectID) {}
	virtual void Reset();

protected:
	std::unordered_map<unsigned int, std::shared_ptr<ObjectBase>> _activeObjects;
	std::vector<std::shared_ptr<ObjectBase>> _removeObjects;

	const unsigned int _objectAmountLimit = 3000;
};

