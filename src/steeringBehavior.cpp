#include "steeringBehavior.h"

#include "enemyBase.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "rayCast.h"
#include "obstacleManager.h"
#include "obstacleWall.h"

AlignBehavior::AlignBehavior() {
	_behaviorType = SteeringBehaviorType::Align;
}

SteeringOutput AlignBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
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
	_targetOrientation = universalFunctions->VectorAsOrientation(objectBase.GetVelocity().absolute());
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
		if (_minSeparation > 2 * _currentCollider->GetRadius()) {
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
}

SteeringOutput ObstacleAvoidanceBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Sets that values for the ray cast and its smaller, rotated rays called whiskers
	_rayPoint.pointHit = false;
	_ray.length = behaviorData.lookAhead;
	_ray.direction = (objectBase.GetVelocity() * _ray.length).normalized();
	_ray.startPosition = objectBase.GetPosition();

	_whiskerA = _ray;
	_whiskerA.length = _ray.length * 0.75f;
	_whiskerA.direction = _ray.direction.rotated(-PI * 0.25);

	_whiskerB = _ray;
	_whiskerB.length = _ray.length * 0.75f;
	_whiskerB.direction = _ray.direction.rotated(PI * 0.25);

	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _ray.direction * _ray.length, {0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _whiskerA.direction * _whiskerA.length, { 0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _whiskerB.direction * _whiskerB.length, { 0, 255, 0, 255 });

	//Goes through the obstacles and see if any of the rays intersects with the obstacle
	for (unsigned int i = 0; i < objectBase.GetQueriedObjects().size(); i++) {
		if (objectBase.GetQueriedObjects()[i]->GetObjectType() != ObjectType::Obstacle) {
			//Makes sure the object is an obstacle and has an AABB collider
			continue;
		}
		_currentCollider = std::static_pointer_cast<AABB>(std::static_pointer_cast<AABB>(objectBase.GetQueriedObjects()[i]->GetCollider()));
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

SeparationBehavior::SeparationBehavior() {
	_behaviorType = SteeringBehaviorType::Separation;
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
		if (_distance < behaviorData.separationThreshold) {
			//Using the inverse quare law to calculate the separation strength
			_strength = std::min(behaviorData.decayCoefficient / (_distance * _distance), behaviorData.maxLinearAcceleration);
			
			//Using linear separation to calculate the separation strength
			//_strength = behaviorData.maxLinearAcceleration * (behaviorData.separationThreshold - _distance) / behaviorData.separationThreshold;
			
			_direction.normalize();
			_result.linearVelocity += _direction * _strength;
		}
	}
	return _result;
}

VelocityMatchBehaviour::VelocityMatchBehaviour() {
	_behaviorType = SteeringBehaviorType::VelocityMatch;
}

SteeringOutput VelocityMatchBehaviour::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Calculates the velocity between the targets and current enemy
	_result.linearVelocity = behaviorData.targetsVelocity - objectBase.GetVelocity();
	_result.linearVelocity /= behaviorData.timeToTarget;

	//Make sure the velocity doesn't get higher than the max acceleration
	universalFunctions->LimitVelocity(_result.linearVelocity, behaviorData.maxLinearAcceleration);
	_result.angularVelocity = 0.f;
	return _result;
}

WanderBehavior::WanderBehavior() {
	_behaviorType = SteeringBehaviorType::Wander;
}

SteeringOutput WanderBehavior::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	//Set the current wander orientation based on a random value combined with the wander rate
	_wanderOrientation += universalFunctions->RandomBinomialFloat(-1, 1) * behaviorData.wanderRate;

	//Calculates the combined orientation between the enemy and the current wanderOrientation
	_targetOrientation = _wanderOrientation + objectBase.GetOrientation();
	
	//Calculates the center position of the target
	_targetPosition = objectBase.GetPosition() + 
		(universalFunctions->OrientationAsVector(objectBase.GetOrientation()) * behaviorData.wanderOffset);

	//Adds the orientation and wanderRadius as an offset to the targetPosition
	objectBase.SetTargetPosition(_targetPosition +
		universalFunctions->OrientationAsVector(behaviorData.targetOrientation) * behaviorData.wanderRadius);

	debugDrawer->AddDebugCross(objectBase.GetTargetPosition(), 25.f, { 0, 255, 0, 255 });

	//Use the faceBehavior to make the enemy look at the new targetPosition
	_result = FaceBehavior::Steering(behaviorData, objectBase);

	//Set the linearVelocity based on the enemies current orientation
	_result.linearVelocity = universalFunctions->OrientationAsVector(objectBase.GetOrientation()) * behaviorData.maxLinearAcceleration;
	return _result;
}

SteeringOutput BlendSteering::Steering(const BehaviorData& behaviorData, ObjectBase& objectBase) {
	_currentWeight = 0.f;
	_result.angularVelocity = 0.f;
	_result.linearVelocity = { 0.f, 0.f };

	//Loops through all the behaviors and adds their results together
	for (int i = 0; i < _behaviors.size(); i++) {
		_currentSteering = _behaviors[i].steeringBehaviour->Steering(behaviorData, objectBase);
		_currentWeight = _behaviors[i].weight;
		_result.linearVelocity += (_currentSteering.linearVelocity * _currentWeight);
		_result.angularVelocity += _currentSteering.angularVelocity * _currentWeight;
	}
	return _result;
}

void BlendSteering::AddSteeringBehaviour(const BehaviorAndWeight& behaviour) {
	_behaviors.emplace_back(behaviour);
}

bool BlendSteering::RemoveSteeringBehaviour(const SteeringBehaviorType& behaviorType) {
	//Loops through and tries to find the target behavior type and removes it if found
	for (unsigned int i = 0; i < _behaviors.size(); i++) {
		if (_behaviors[i].steeringBehaviour->GetBehaviorType() == behaviorType) {
			_behaviors.erase(_behaviors.begin() + i);
			return true;
		}
	}
	return false;
}

void BlendSteering::ClearBehaviours() {
	_behaviors.clear();
}

bool BlendSteering::ReplaceSteeringBheavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior) {
	//Loops through and tries to find the target behavior type and replaces it with the new be
	for (unsigned int i = 0; i < _behaviors.size(); i++) {
		if (_behaviors[i].steeringBehaviour->GetBehaviorType() == oldBehaviorType) {
			_behaviors[i] = newBehavior;
			return true;
		}
	}
	//If the behavior is not found, it returns false
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

void PrioritySteering::AddGroup(const BlendSteering& behaviour) {
	_groups.emplace_back(behaviour);
}

void PrioritySteering::ClearGroups() {
	_groups.clear();
}

void PrioritySteering::AddBehaviorInGroup(const BehaviorAndWeight& newBehavior, const int& groupIndex) {
	_groups[groupIndex].AddSteeringBehaviour(newBehavior);
}

bool PrioritySteering::RemoveSteeringBheavior(const SteeringBehaviorType& oldBehaviorType) {
	for (unsigned int i = 0; i < _groups.size(); i++) {
		if (_groups[i].RemoveSteeringBehaviour(oldBehaviorType)) {
			return true;
		}
	}
	return false;
}

bool PrioritySteering::ReplaceSteeringBheavior(const SteeringBehaviorType& oldBehaviorType, const BehaviorAndWeight& newBehavior) {
	for (unsigned int i = 0; i < _groups.size(); i++) {
		if (_groups[i].ReplaceSteeringBheavior(oldBehaviorType, newBehavior)) {
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
