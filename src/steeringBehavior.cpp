#include "steeringBehavior.h"

#include "debugDrawer.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "managerBase.h"
#include "objectBase.h"
#include "obstacleManager.h"
#include "obstacleWall.h"
#include "quadTree.h"
#include "rayCast.h"
#include "universalFunctions.h"

AlignBehavior::AlignBehavior() {
	_behaviorType = SteeringBehaviorType::Align;
}

SteeringOutput AlignBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	if (fabs(_targetOrientation) <= FLT_EPSILON) {
		_targetOrientation = behaviorData.targetOrientation;
	}
	//Calculates the speed that the enemy will rotate and clamps it between -PI, PI radians(-180, 180 degree)
	_rotation = _targetOrientation - objectBase.GetOrientation();
	_rotation = universalFunctions->WrapMinMax(_rotation, -PI, PI);

	//Uses a variable that will always be the positive value and use it for the if statements
	_rotationSize = abs(_rotation);

	//Compare the rotationSize value with targetRadius and slowDownRadius
	if (_rotationSize < behaviorData.angularTargetRadius) {
		//If the rotationSize is less than the angularTargetRadius, it means that the enemy doesn't need to rotate
		return SteeringOutput();
	}
	if (_rotationSize > behaviorData.angularSlowDownRadius) {
		//If rotationSize is larger than slowDownRadius, the enemy will rotate based on the maxRotation
		_wantedRotation = behaviorData.maxRotation;
	} else {
		//If rotationSize is less than slowDownRadius, calculate the rotation scaled by the rotationSize and slowDownRadius
		_wantedRotation = behaviorData.maxRotation * _rotationSize / behaviorData.angularSlowDownRadius;
	}
	//Multiplies wantedRotation with the direction of the rotation
	_wantedRotation *= _rotation / _rotationSize;

	//Calculates the angularVelocity the enemy will rotate compared to its current rotation
	_result.angularVelocity = _wantedRotation - objectBase.GetRotation();
	_result.angularVelocity /= behaviorData.timeToTarget;

	//Makes sure the angular acceleration doesn't go beyond the maxAngularAcceleration
	_angularAcceleration = abs(_result.angularVelocity);
	if (_angularAcceleration > behaviorData.maxAngularAcceleration) {
		_result.angularVelocity /= _angularAcceleration;
		_result.angularVelocity *= behaviorData.maxAngularAcceleration;
	}
	//Make sure to return the linearVelocity as 0, since this behavior will only change the angularVelocity
	_result.linearVelocity = Vector2(0.f, 0.f);
	_targetOrientation = 0.f;
	return _result;
}

FaceBehavior::FaceBehavior() {
	_behaviorType = SteeringBehaviorType::Face;
}

SteeringOutput FaceBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_direction = objectBase.GetTargetPosition() - objectBase.GetPosition();
	if (_direction.absolute() == 0) {
		return SteeringOutput();
	}
	//Sets the targetOrientation based on the direction using atan2f
	_targetOrientation = universalFunctions->VectorAsOrientation(_direction);

	//returns the steering function from alignBehavior which aligns the character with the targetOrientation
	return AlignBehavior::Steering(behaviorData, objectBase);
}
LookAtDirectionBehavior::LookAtDirectionBehavior() {}

SteeringOutput LookAtDirectionBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//If the enemy doesn't have a veolcity, return 0
	if (objectBase.GetVelocity().absolute() == 0) {
		return SteeringOutput();
	}
	//Sets the targetOrientation to the velocity as orientation using atan2f
	_targetOrientation = universalFunctions->VectorAsOrientation(objectBase.GetVelocity());
	return AlignBehavior::Steering(behaviorData, objectBase);
}

ArriveBehavior::ArriveBehavior() {
	_behaviorType = SteeringBehaviorType::Arrive;
}

SteeringOutput ArriveBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Calculates the direction and distance between target and enemy
	_direction = objectBase.GetTargetPosition() - objectBase.GetPosition();
	_distance = _direction.absolute();
	
	//If distance is in target radius, the behavior will return and not update the behavior further
	if (_distance < behaviorData.linearTargetRadius) {
		return SteeringOutput();
	}
	if (_distance > behaviorData.linearSlowDownRadius) {
		//If distance is larger than the slowDownRadius, set the targetSpeed to maxSpeed
		_targetSpeed = behaviorData.maxSpeed;
	} else {
		//If distance is smaller than slowDownRadius, calculate targetSpeed scaled by the distance and slowDownRadius
		_targetSpeed = behaviorData.maxSpeed * _distance / behaviorData.linearSlowDownRadius;
	}
	//Normalize the direction and combine targetVelocity with the targetSpeed
	_targetVelocity = _direction.normalized();
	_targetVelocity *= _targetSpeed;

	//Set the difference between the targetVelocity and the enemies current velocity as the resulted velocity
	_result.linearVelocity = _targetVelocity - objectBase.GetVelocity();
	_result.linearVelocity /= behaviorData.timeToTarget;

	//Make sure the velocity doesn't go beyond maxAcceleration
	universalFunctions->LimitVelocity(_result.linearVelocity, behaviorData.maxLinearAcceleration);

	//Make sure to return the angularVelocity as 0, since this behavior will only change the linearVelocity
	_result.angularVelocity = 0.f;
	return _result;
}

CollisionAvoidanceBehavior::CollisionAvoidanceBehavior() {
	_behaviorType = SteeringBehaviorType::CollisionAbvoidance;
}

SteeringOutput CollisionAvoidanceBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_currentCollider = std::static_pointer_cast<Circle>(objectBase.GetCollider());

	//Gets all objectBase that has collided with this enemy through the quadTree
	for (unsigned int i = 0; i < objectBase.GetQueriedObjects().size(); i++) {
		//If the object is not an enemy or it's the same as the current enemy, it will continue to the next object
		if (objectBase.GetQueriedObjects()[i]->GetObjectType() != ObjectType::Enemy ||
			objectBase.GetObjectID() == objectBase.GetQueriedObjects()[i]->GetObjectID()) {
			continue;
		}
		//Calculate the relative speed, position and velocity between current enemy and targetEnemy
		_relativePos = objectBase.GetQueriedObjects()[i]->GetPosition() - objectBase.GetPosition();
		_relativeVel = objectBase.GetQueriedObjects()[i]->GetVelocity() - objectBase.GetVelocity();
		_relativeSpeed = _relativeVel.absolute();

		//Calculates the time to collision betwwen the current enemy and targetEnemy
		_timeToCollision = Vector2<float>::dotProduct(_relativePos, _relativeVel) / (_relativeSpeed * _relativeSpeed);
		
		//See if a collision is going to happen
		_distance = _relativePos.absolute();
		_minSeparation = _distance - _relativeSpeed * _timeToCollision;

		/*If minSeparation is larger than colliders diameter,
		then the enemies won't collide and collisionAvoidance is not need*/
		if (_minSeparation > _currentCollider->GetRadius() * 5.f) {
			continue;
		}
		//If the current timeToCollision is the shortest, set the targetEnemy as the first target
		if (_timeToCollision < _shortestTime) {
			_shortestTime = _timeToCollision;
			_firstTargetPosition = objectBase.GetQueriedObjects()[i]->GetPosition();
			_firstMinSeparation = _minSeparation;
			_firstDistance = _distance;
			_firstRelativePos = _relativePos;
			_firstRelativeVel = _relativeVel;
			_gotTarget = true;
		}
	}
	//If there is not target, return 0 since there is no target to avoid
	if (!_gotTarget) {
		return SteeringOutput();
	}

	//If they are already colliding, base the steering of the current position
	if (_firstMinSeparation <= 0 || _firstDistance < 2 * _currentCollider->GetRadius()) {
		_relativePos = objectBase.GetPosition() - _firstTargetPosition;
	} else {
		//Calculates the future relativePosition
		_relativePos = _firstRelativePos + _firstRelativeVel * _shortestTime;
	}
	_relativePos.normalize();
	_gotTarget = false;
	_shortestTime = FLT_MAX;

	//Avoid the target by returning acceleration beased on the relativePosition
	_result.linearVelocity = _relativePos * behaviorData.maxLinearAcceleration;

	_result.angularVelocity = 0.f;
	return _result;
}

SeekBehavior::SeekBehavior(const SteeringBehaviorType& behaviorType) {
	_behaviorType = behaviorType;
}

SteeringOutput SeekBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	if (_behaviorType == SteeringBehaviorType::Evade || _behaviorType == SteeringBehaviorType::Flee) {
		//If the target is using the seek behavior, calculate the velocity from the targetPosition and enemyPosition
		_result.linearVelocity = objectBase.GetPosition() - objectBase.GetTargetPosition();

	} else if (_behaviorType == SteeringBehaviorType::Pursue || _behaviorType == SteeringBehaviorType::Seek) {
		//If the target is using the flee behavior, calculate the velocity from the enemyPosition and targetPosition
		_result.linearVelocity = objectBase.GetTargetPosition() - objectBase.GetPosition();

	} else {
		//If an unrelated behavior has been chosen, use seek as default
		_result.linearVelocity = objectBase.GetTargetPosition() - objectBase.GetPosition();
	}

	//Set the velocity based on the maxAcceleration
	_result.linearVelocity = _result.linearVelocity.normalized();
	_result.linearVelocity *= behaviorData.maxLinearAcceleration;

	//Make sure the velocity doesn't go above the maxSpeed
	universalFunctions->LimitVelocity(_result.linearVelocity, behaviorData.maxSpeed);

	//Make sure to return the angularVelocity as 0, since this behavior will only change the linearVelocity
	_result.angularVelocity = 0.f;
	return _result;
}

ObstacleAvoidanceBehavior::ObstacleAvoidanceBehavior() : SeekBehavior(SteeringBehaviorType::Seek) {
	_behaviorType = SteeringBehaviorType::ObstacleAvoidance;
	_circleCollider = std::make_shared<Circle>();
}

SteeringOutput ObstacleAvoidanceBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Sets that values for the ray cast and its smaller, rotated rays called whiskers
	_rayPoint.pointHit = false;
	_ray.length = behaviorData.lookAhead;
	_ray.direction = (objectBase.GetVelocity() * _ray.length).normalized();
	_ray.startPosition = objectBase.GetPosition();

	_whiskerA = _ray;
	_whiskerA.length = _ray.length * 0.5f;
	_whiskerA.direction = _ray.direction.rotated(-PI * 0.125f);

	_whiskerB = _ray;
	_whiskerB.length = _ray.length * 0.5f;
	_whiskerB.direction = _ray.direction.rotated(PI * 0.125f);

	//debugDrawer->AddDebugLine(objectBase.GetPosition(), objectBase.GetPosition() + _ray.direction * _ray.length, { 0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(objectBase.GetPosition(), objectBase.GetPosition() + _whiskerA.direction * _whiskerA.length, { 0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(objectBase.GetPosition(), objectBase.GetPosition() + _whiskerB.direction * _whiskerB.length, { 0, 255, 0, 255 });

	_circleCollider->Init(objectBase.GetPosition(), behaviorData.lookAhead);
	_queriedObstacles = obstacleQuadTree->Query(_circleCollider);

	//Goes through the obstacles and see if any of the rays intersects with the obstacle
	for (std::shared_ptr<ObjectBase> object : _queriedObstacles) {
		if (object->GetCollider()->GetColliderType() != ColliderType::AABB) {
			//Makes sure the object is an obstacle and has an AABB collider
			continue;
		}
		_currentCollider = std::static_pointer_cast<AABB>(std::static_pointer_cast<AABB>(object->GetCollider()));
		//If any of the rays hit, break the loop
		_rayPoint = rayCast->RayCastToAABB(_currentCollider, _ray);
		if (_rayPoint.pointHit) {
			break;
		}
		_rayPoint = rayCast->RayCastToAABB(_currentCollider, _whiskerA);
		if (_rayPoint.pointHit) {
			break;
		}
		_rayPoint = rayCast->RayCastToAABB(_currentCollider, _whiskerB);
		if (_rayPoint.pointHit) {
			break;
		}

	}
	//If no object is intersecting, return 0
	if (!_rayPoint.pointHit) {
		return SteeringOutput();
	}
	//When the raypoint intersects, set the targetPosition based on the rayPoint position, its normal combined with the avoidanceDistance 
	objectBase.SetTargetPosition(_rayPoint.position + _rayPoint.normal * behaviorData.avoidDistance);

	//Avoid the object by using seekBehavior toward the target position
	return SeekBehavior::Steering(behaviorData, objectBase);
}
PursueBehavior::PursueBehavior(const SteeringBehaviorType& behaviorType) : SeekBehavior(behaviorType) {
	_behaviorType = behaviorType;
}

SteeringOutput PursueBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Calculate the distance between enemy and target 
	_distance = (objectBase.GetPosition() - objectBase.GetTargetPosition()).absolute();
	//Set the current speed the enemy is travelling
	_speed = objectBase.GetVelocity().absolute();

	//Checks if the speed gives a reasonable predction time
	if (_speed <= _distance / behaviorData.maxPrediction) {
		_prediction = behaviorData.maxPrediction;
	} else {
		//If not, calculate the prediction time based on the distance and speed
		_prediction = _distance / _speed;
	}
	//Sets the targetPosition to the targets position added with the targets velocity multiplied with the calculated prediction
	objectBase.SetTargetPosition(objectBase.GetTargetPosition() + (behaviorData.targetsVelocity * _prediction));
	//Runs the seek steering behavior with the new calculated targetPosition
	return SeekBehavior::Steering(behaviorData, objectBase);
}

SeparationBehavior::SeparationBehavior(const SteeringBehaviorType& behaviorType) {
	_behaviorType = behaviorType;
}

SteeringOutput SeparationBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_result.angularVelocity = 0.f;
	_result.linearVelocity = { 0.f, 0.f };

	//Loops through all objects detected by the quadtree
	for (unsigned int i = 0; i < objectBase.GetQueriedObjects().size(); i++) {
		if (objectBase.GetQueriedObjects()[i]->GetObjectType() == ObjectType::Projectile) {
			continue;
		}
		//Skips if the enemy in the loop is the same one as the current one
		if (objectBase.GetObjectID() == objectBase.GetQueriedObjects()[i]->GetObjectID()) {
			continue;
		}
		//Sets the targetPosition and calculate the direction and distance based on the current targetEnemy
		_targetPosition = objectBase.GetQueriedObjects()[i]->GetPosition();
		_direction = objectBase.GetPosition() - _targetPosition;
		_distance = _direction.absolute();
		switch (_behaviorType) {
		case SteeringBehaviorType::Attraction:
			if (_distance > behaviorData.attractionThreshold) {
				_strength = std::min(behaviorData.decayCoefficient / (_distance * _distance), behaviorData.maxLinearAcceleration);
				_direction.normalize();
				_result.linearVelocity -= _direction * _strength;
			}
			break;

		case SteeringBehaviorType::Separation:
			if (_distance < behaviorData.separationThreshold) {
				//Using the inverse quare law to calculate the separation strength
				_strength = std::min(behaviorData.decayCoefficient / (_distance * _distance), behaviorData.maxLinearAcceleration);	
				_direction.normalize();
				_result.linearVelocity += _direction * _strength;
			}
			break;

		default:
			break;
		}
	}
	return _result;
}

VelocityMatchBehavior::VelocityMatchBehavior() {
	_behaviorType = SteeringBehaviorType::VelocityMatch;
}

SteeringOutput VelocityMatchBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Calculates the velocity between the targets and current enemy
	_result.linearVelocity = behaviorData.targetsVelocity - objectBase.GetVelocity();
	_result.linearVelocity /= behaviorData.timeToTarget;

	//Make sure the velocity doesn't get higher than the max acceleration
	universalFunctions->LimitVelocity(_result.linearVelocity, behaviorData.maxLinearAcceleration);
	_result.angularVelocity = 0.f;
	return _result;
}

WanderBehavior::WanderBehavior() : SeekBehavior(SteeringBehaviorType::Seek) {
	SteeringBehavior::_behaviorType = SteeringBehaviorType::Wander;
}

SteeringOutput WanderBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_randomNumber = universalFunctions->RandomBinomialFloat(-1, 1) * behaviorData.wanderRate;
	//Set the current wander orientation based on a random value combined with the wander rate
	_wanderOrientation += _randomNumber;

	//Calculates the combined orientation between the enemy and the current wanderOrientation
	_targetOrientation = _wanderOrientation + objectBase.GetOrientation();

	//Calculates the center position of the target
	_targetPosition = objectBase.GetPosition() - (universalFunctions->OrientationAsVector(objectBase.GetOrientation()) * behaviorData.wanderOffset);

	//If the target is near the border, add rotation based to prevent it from going outside
	if (universalFunctions->OutsideBorderX(_targetPosition.x, _outOfBorderOffset) || universalFunctions->OutsideBorderY(_targetPosition.y, _outOfBorderOffset)) {
		if (objectBase.GetRotation() >= 0) {
			objectBase.SetRotation(objectBase.GetRotation() + behaviorData.maxRotation * deltaTime);

		} else {
			objectBase.SetRotation(objectBase.GetRotation() - behaviorData.maxRotation * deltaTime);
		}
	}

	//Adds the orientation and wanderRadius as an offset to the targetPosition
	objectBase.SetTargetPosition(_targetPosition - (universalFunctions->OrientationAsVector(_targetOrientation) * behaviorData.wanderRadius));
	
	debugDrawer->AddDebugCross(objectBase.GetTargetPosition(), 15.f, { 0, 255, 0, 255 });
	debugDrawer->AddDebugCircle(_targetPosition, behaviorData.wanderRadius, { 0, 255,0,255 });

	return { SeekBehavior::Steering(behaviorData, objectBase).linearVelocity, FaceBehavior::Steering(behaviorData, objectBase).angularVelocity };
}

SteeringOutput BlendSteering::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_result.angularVelocity = 0.f;
	_result.linearVelocity = { 0.f, 0.f };

	for (auto& behavior : _behaviorsMap) {
		_currentSteering = behavior.second.steeringBehavior->Steering(behaviorData, objectBase);
		_result.linearVelocity += (_currentSteering.linearVelocity * behavior.second.weight);
		_result.angularVelocity += _currentSteering.angularVelocity * behavior.second.weight;
	}
	return _result;
}

void BlendSteering::AddSteeringBehavior(const BehaviorAndWeight& behavior) {
	if (!_behaviorsMap.contains(behavior.steeringBehavior->GetBehaviorType())) {
		_behaviorsMap.insert(std::make_pair(behavior.steeringBehavior->GetBehaviorType(), behavior));
	}
}

bool BlendSteering::RemoveSteeringBehavior(const SteeringBehaviorType& behaviorType) {
	if (_behaviorsMap.contains(behaviorType)) {
		_behaviorsMap.erase(behaviorType);
		return true;
	}
	return false;
}

void BlendSteering::ClearBehaviors() {
	_behaviorsMap.clear();
}

bool BlendSteering::ReplaceSteeringBehavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior) {
	if (_behaviorsMap.contains(oldBehaviorType)) {
		_behaviorsMap.erase(oldBehaviorType);
		_behaviorsMap.insert(std::make_pair(newBehavior.steeringBehavior->GetBehaviorType(), newBehavior));
		return true;
	}
	return false;
}

SteeringOutput PrioritySteering::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_result.linearVelocity = { 0.f, 0.f };
	_result.angularVelocity = 0.f;

	for (unsigned int i = 0; i < _groups.size(); i++) {
		_result = _groups[i].Steering(behaviorData, objectBase);
		//If the current group of behaviors is in use, it will use this result and not loop through the next groups
		if (_result.linearVelocity.absolute() > FLT_EPSILON || abs(_result.angularVelocity) > FLT_EPSILON) {
			//This is useful for behaviors like collision avoidance where it should have the priority over other behaviors
			return _result;
		}
	}
	return _result;
}

void PrioritySteering::AddGroup(const BlendSteering& behaviors) {
	_groups.emplace_back(behaviors);
}

void PrioritySteering::ClearGroups() {
	_groups.clear();
}

void PrioritySteering::AddBehaviorInGroup(const BehaviorAndWeight& newBehavior, const int& groupIndex) {
	_groups[groupIndex].AddSteeringBehavior(newBehavior);
}

bool PrioritySteering::RemoveSteeringBheavior(const SteeringBehaviorType& oldBehaviorType) {
	for (unsigned int i = 0; i < _groups.size(); i++) {
		if (_groups[i].RemoveSteeringBehavior(oldBehaviorType)) {
			return true;
		}
	}
	return false;
}

bool PrioritySteering::ReplaceSteeringBehavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior) {
	for (unsigned int i = 0; i < _groups.size(); i++) {
		if (_groups[i].ReplaceSteeringBehavior(oldBehaviorType, newBehavior)) {
			return true;
		}
	}
	return false;
}

SteeringOutput SteeringBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	return SteeringOutput();
}

const SteeringBehaviorType SteeringBehavior::GetBehaviorType() const {
	return _behaviorType;
}
