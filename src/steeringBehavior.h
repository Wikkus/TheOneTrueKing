#pragma once
#include "enemyManager.h"
#include "rayCast.h"
#include "vector2.h"

#include <memory>

class Collider;
class EnemyBase;

enum class SteeringBehaviorType {
	Align,
	Arrive,
	Attraction,
	CollisionAbvoidance,
	Evade,
	Face,
	Flee,
	LookAtDirection,
	ObstacleAvoidance,
	Pursue,
	Seek,
	Separation,
	VelocityMatch,
	Wander,
	Count
};

struct SteeringOutput {
	Vector2<float> linearVelocity = Vector2<float>(0, 0);
	float angularVelocity = 0.f;
};

class SteeringBehavior {
public:
	SteeringBehavior() {}
	~SteeringBehavior() {}
	virtual SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase);
	virtual const SteeringBehaviorType GetBehaviorType() const;


protected:
	SteeringBehaviorType _behaviorType = SteeringBehaviorType::Count;
	SteeringOutput _result;
};

class AlignBehavior : public SteeringBehavior {
public:
	AlignBehavior();
	~AlignBehavior() {}
	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

protected:
	float _targetOrientation = 0.f;

private:
	float _angularAcceleration = 0.f;
	float _wantedRotation = 0.f;
	float _rotation = 0.f;
	float _rotationSize = 0.f;
};
class FaceBehavior : public AlignBehavior {
public:
	FaceBehavior();
	~FaceBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	Vector2<float> _direction = { 0.f, 0.f };

};
class LookAtDirectionBehavior : public AlignBehavior {
public:
	LookAtDirectionBehavior();
	~LookAtDirectionBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;
};

class ArriveBehavior : public SteeringBehavior {
public:
	ArriveBehavior();
	~ArriveBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	Vector2<float> _targetVelocity = { 0.f, 0.f };
	Vector2<float> _direction = { 0.f, 0.f };

	float _targetSpeed = 0.f;
	float _distance = 0.f;
};

class CollisionAvoidanceBehavior : public SteeringBehavior {
public:
	CollisionAvoidanceBehavior();
	~CollisionAvoidanceBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	std::shared_ptr<Circle> _currentCollider = nullptr;

	bool _gotTarget = false;

	Vector2<float> _firstTargetPosition = { 0.f, 0.f };
	Vector2<float> _firstRelativePos = { 0.f, 0.f };
	Vector2<float> _firstRelativeVel = { 0.f, 0.f };

	Vector2<float> _relativePos = { 0.f, 0.f };
	Vector2<float> _relativeVel = { 0.f, 0.f };

	float _firstMinSeparation = 0.f;
	float _firstDistance = 0.f;

	float _relativeSpeed = 0.f;
	float _timeToCollision = 0.f;
	float _distance = 0.f;
	float _minSeparation = 0.f;

	float _shortestTime = FLT_MAX;
};

class SeekBehavior : public SteeringBehavior {
public:
	SeekBehavior(const SteeringBehaviorType& behaviorType);
	~SeekBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

};
class ObstacleAvoidanceBehavior : public SeekBehavior {
public:
	ObstacleAvoidanceBehavior();
	~ObstacleAvoidanceBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	std::shared_ptr<AABB> _currentCollider = nullptr;
	Ray _ray;
	Ray _whiskerA;
	Ray _whiskerB;
	RayPoint _rayPoint;
};
class PursueBehavior : public SeekBehavior {
public:
	PursueBehavior(const SteeringBehaviorType& behaviorType);
	~PursueBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	float _distance = 0.f;
	float _speed = 0.f;
	float _prediction = 0.f;
};

class SeparationBehavior : public SteeringBehavior {
public:
	SeparationBehavior();
	~SeparationBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	Vector2<float> _targetPosition = Vector2<float>(0, 0);
	Vector2<float> _direction = Vector2<float>(0, 0);
	float _distance = 0;
	float _strength = 0.f;
};

class VelocityMatchBehaviour : public SteeringBehavior {
public:
	VelocityMatchBehaviour();
	~VelocityMatchBehaviour() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;
};

class WanderBehavior : public FaceBehavior {
public:
	WanderBehavior();
	~WanderBehavior() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) override;

private:
	Vector2<float> _targetPosition = { 0.f, 0.f };
	float _wanderOrientation = 0;

};

struct BehaviorAndWeight {
	std::shared_ptr<SteeringBehavior> steeringBehaviour = nullptr;
	float weight = 1.f;
};

class BlendSteering {
public:
	BlendSteering() {}
	~BlendSteering() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase);

	void AddSteeringBehaviour(const BehaviorAndWeight& behaviour);	
	void ClearBehaviours();
	
	bool RemoveSteeringBehaviour(const SteeringBehaviorType& behaviorType);
	bool ReplaceSteeringBheavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior);

private:
	float _currentWeight = 0.f;
	SteeringOutput _result;
	SteeringOutput _currentSteering;
	std::vector<BehaviorAndWeight> _behaviors;
};

class PrioritySteering {
public:
	PrioritySteering() {}
	~PrioritySteering() {}

	SteeringOutput Steering(const BehaviorData& behaviorData, ObjectBase& objectBase);

	void AddGroup(const BlendSteering& behavior);
	void ClearGroups();
	
	void AddBehaviorInGroup(const BehaviorAndWeight& newBehavior, const int& groupIndex);
	
	bool RemoveSteeringBheavior(const SteeringBehaviorType& oldBehaviorType);
	bool ReplaceSteeringBheavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior);

private:
	std::vector<BlendSteering> _groups;
	SteeringOutput _result;
};