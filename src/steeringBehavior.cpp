#include "steeringBehavior.h"

#include "dataStructuresAndMethods.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "imGuiManager.h"
#include "rayCast.h"
#include "obstacleManager.h"
#include "obstacleWall.h"


SteeringOutput AlignBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_rotation = behaviorData.targetOrientation - enemy.GetOrientation();
	_rotation = WrapMinMax(_rotation, -PI, PI);
	_rotationSize = abs(_rotation);

	if (_rotationSize < behaviorData.angularTargetRadius) {
		return SteeringOutput();
	}
	if (_rotationSize > behaviorData.angularSlowDownRadius) {
		_wantedRotation = behaviorData.maxRotation;
	} else {
		_wantedRotation = behaviorData.maxRotation * _rotationSize / behaviorData.angularSlowDownRadius;
	}
	_wantedRotation *= _rotation / _rotationSize;

	_result.angularVelocity = _wantedRotation - enemy.GetRotation();
	_result.angularVelocity /= behaviorData.timeToTarget;

	_angularAcceleration = abs(_result.angularVelocity);
	if (_angularAcceleration > behaviorData.maxAngularAcceleration) {
		_result.angularVelocity /= _angularAcceleration;
		_result.angularVelocity *= behaviorData.maxAngularAcceleration;
	}
	_result.linearVelocity = Vector2(0.f, 0.f);
	return _result;
}

SteeringOutput FaceBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_direction = enemy.GetBehaviorData().targetPosition - enemy.GetPosition();
	if (_direction.absolute() == 0) {
		return SteeringOutput();
	}
	behaviorData.targetOrientation = atan2f(_direction.x, -_direction.y);
	return AlignBehavior::Steering(behaviorData, enemy);
}
SteeringOutput LookAtDirectionBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	Vector2 velocity = enemy.GetVelocity();
	if (velocity.absolute() == 0) {
		return SteeringOutput();
	}
	behaviorData.targetOrientation = atan2f(-velocity.x, velocity.y);
	return AlignBehavior::Steering(behaviorData, enemy);
}

SteeringOutput ArriveBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_targetVelocity = { 0, 0 };
	_direction = enemy.GetBehaviorData().targetPosition - enemy.GetPosition();

	_targetSpeed = 0.f;
	_distance = _direction.absolute();

	if (_distance < behaviorData.linearTargetRadius) {
		enemy.SetVelocity({ 0.f, 0.f });
		return SteeringOutput();
	}
	if (_distance > behaviorData.linearSlowDownRadius) {
		_targetSpeed = behaviorData.maxSpeed;
	} else {
		_targetSpeed = behaviorData.maxSpeed * _distance / behaviorData.linearSlowDownRadius;
	}
	_targetVelocity = _direction.normalized();
	_targetVelocity *= _targetSpeed;

	_result.linearVelocity = _targetVelocity - enemy.GetVelocity();
	_result.linearVelocity /= behaviorData.timeToTarget;

	if (_result.linearVelocity.absolute() > behaviorData.maxLinearAcceleration) {
		_result.linearVelocity.normalize();
		_result.linearVelocity *= behaviorData.maxLinearAcceleration;
	}
	_result.angularVelocity = 0.f;
	return _result;
}

SteeringOutput CollisionAvoidanceBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	for (unsigned int i = 0; i < enemy.GetQueriedObjects().size(); i++) {
		if (enemy.GetQueriedObjects()[i]->GetObjectType() != ObjectType::Enemy) {
			continue;
		}
		if (enemy.GetObjectID() == enemy.GetQueriedObjects()[i]->GetObjectID()) {
			continue;
		}
		std::shared_ptr<EnemyBase> targetEnemy = std::static_pointer_cast<EnemyBase>(enemy.GetQueriedObjects()[i]);

		_direction = targetEnemy->GetPosition() - enemy.GetPosition();

		_relativePos = targetEnemy->GetPosition() - enemy.GetPosition();
		_relativeVel = targetEnemy->GetVelocity() - enemy.GetVelocity();

		_relativeSpeed = _relativeVel.absolute();
		_timeToCollision = Vector2<float>::dotProduct(_relativePos, _relativeVel) / (_relativeSpeed * _relativeSpeed);
		
		_distance = _relativePos.absolute();
		_minSeparation = _distance - _relativeSpeed * _timeToCollision;

		if (_minSeparation > 2 * enemy.GetCollider().radius) {
			continue;
		}
		if (_timeToCollision < _shortestTime) {
			_shortestTime = _timeToCollision;
			_firstTargetPosition = enemy.GetQueriedObjects()[i]->GetPosition();
			_firstMinSeparation = _minSeparation;
			_firstDistance = _distance;
			_firstRelativePos = _relativePos;
			_firstRelativeVel = _relativeVel;
			_gotTarget = true;
		}
	}

	if (!_gotTarget) {
		return SteeringOutput();
	}
	if (_firstMinSeparation <= 0 || _firstDistance < 2 * enemy.GetCollider().radius) {
		_relativePos = enemy.GetPosition() - _firstTargetPosition;
	} else {
		_relativePos = _firstRelativePos + _firstRelativeVel * _shortestTime;
	}
	_relativePos.normalize();
	_gotTarget = false;

	_result.linearVelocity = _relativePos * behaviorData.maxLinearAcceleration;
	_result.angularVelocity = 0.f;
	return _result;
}

SteeringOutput SeekBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	if (_fleeBehaviour) {
		_result.linearVelocity = enemy.GetPosition() - enemy.GetBehaviorData().targetPosition;
	} else {
		_result.linearVelocity = enemy.GetBehaviorData().targetPosition - enemy.GetPosition();
	}
	_result.linearVelocity = _result.linearVelocity.normalized();
	_result.linearVelocity *= behaviorData.maxLinearAcceleration;

	if (_result.linearVelocity.absolute() > behaviorData.maxSpeed) {
		_result.linearVelocity = _result.linearVelocity.normalized();
		_result.linearVelocity *= behaviorData.maxSpeed;
	}
	_result.angularVelocity = 0.f;
	return _result;
}
SteeringOutput ObstacleAvoidanceBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_rayPoint.pointHit = false;
	_ray.length = behaviorData.lookAhead;
	_ray.direction = (enemy.GetVelocity() * _ray.length).normalized();
	_ray.startPosition = enemy.GetPosition();

	_whiskerA = _ray;
	_whiskerA.length = _ray.length * 0.75f;
	_whiskerA.direction = RotateVector(-PI * 0.25, enemy.GetPosition(), enemy.GetPosition() + _ray.direction);
	
	_whiskerB = _ray;
	_whiskerB.length = _ray.length * 0.75f;
	_whiskerB.direction = RotateVector(PI * 0.25, enemy.GetPosition(), enemy.GetPosition() + _ray.direction);

	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _ray.direction * _ray.length, {0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _whiskerA.direction * _whiskerA.length, { 0, 255, 0, 255 });
	//debugDrawer->AddDebugLine(enemy->GetPosition(), enemy->GetPosition() + _whiskerB.direction * _whiskerB.length, { 0, 255, 0, 255 });

	for (int i = 0; i < obstacleManager->GetWalls().size(); i++) {
		_rayPoint = rayCast->RayCastToAABB(obstacleManager->GetWalls()[i]->GetCollider(), _ray);
		if (_rayPoint.pointHit) {
			break;
		}
		_rayPoint = rayCast->RayCastToAABB(obstacleManager->GetWalls()[i]->GetCollider(), _whiskerA);
		if (_rayPoint.pointHit) {
			break;
		}
		_rayPoint = rayCast->RayCastToAABB(obstacleManager->GetWalls()[i]->GetCollider(), _whiskerB);
		if (_rayPoint.pointHit) {
			break;
		}
	}
	if (!_rayPoint.pointHit) {		
		return SteeringOutput();
	}
	behaviorData.targetPosition = _rayPoint.position + _rayPoint.normal * behaviorData.avoidDistance;
	return SeekBehavior::Steering(behaviorData, enemy);
}
SteeringOutput PursueBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_direction = enemy.GetPosition() - enemy.GetBehaviorData().targetPosition;
	_distance = _direction.absolute();
	_speed = enemy.GetVelocity().absolute();

	if (_speed <= _distance / behaviorData.maxPrediction) {
		_prediction = behaviorData.maxPrediction;
	} else {
		_prediction = _distance / _speed;
	}
	behaviorData.targetPosition = enemy.GetBehaviorData().targetPosition + (behaviorData.targetVelocity * _prediction);

	return SeekBehavior::Steering(behaviorData, enemy);
}

SteeringOutput SeparationBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_result.angularVelocity = 0.f;
	_result.linearVelocity = { 0.f, 0.f };

	//Loops through all enemies detected by the quadtree
	for (unsigned int i = 0; i < enemy.GetQueriedObjects().size(); i++) {
		if (enemy.GetQueriedObjects()[i]->GetObjectType() == ObjectType::Projectile) {
			continue;
		}
		//Skips if the enemy in the loop is the same one as the current one
		if (enemy.GetObjectID() == enemy.GetQueriedObjects()[i]->GetObjectID()) {
			continue;
		}
		_targetPosition = enemy.GetQueriedObjects()[i]->GetPosition();
		_direction = _targetPosition - enemy.GetPosition();
		_distance = _direction.absolute();
		//If the enemies are closer to each other than the threshold,
		if (_distance < behaviorData.separationThreshold) {
			//I'm setting the strength to the lowest value between decayCoefficient * distance^2 and max acceleration
			//This makes the strength stronger the closer they are to each other but doesn't go beyond max acceleration
			_strength = std::min(behaviorData.decayCoefficient / (_distance * _distance), behaviorData.maxLinearAcceleration);
			_direction.normalize();
			_result.linearVelocity -= _direction * _strength;
		}
	}
	return _result;
}

SteeringOutput VelocityMatchBehaviour::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_result.linearVelocity = behaviorData.targetVelocity - enemy.GetVelocity();
	_result.linearVelocity /= behaviorData.timeToTarget;

	if (_result.linearVelocity.absolute() > behaviorData.maxLinearAcceleration) {
		_result.linearVelocity = _result.linearVelocity.normalized();
		_result.linearVelocity *= behaviorData.maxLinearAcceleration;
	}
	_result.angularVelocity = 0.f;
	return _result;
}

SteeringOutput WanderBehavior::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_wanderOrientation = RandomBinomal(-1, 1) * behaviorData.wanderRate;

	behaviorData.targetOrientation = _wanderOrientation + enemy.GetOrientation();
	
	behaviorData.targetPosition = enemy.GetPosition() +
		(OrientationAsVector(enemy.GetOrientation()) * behaviorData.wanderOffset);

	behaviorData.targetPosition += OrientationAsVector(behaviorData.targetOrientation) * behaviorData.wanderRadius;
	debugDrawer->AddDebugCross(behaviorData.targetPosition, 25.f, { 0, 255, 0, 255 });

	_result = FaceBehavior::Steering(behaviorData, enemy);
	_result.linearVelocity = OrientationAsVector(enemy.GetOrientation()) * behaviorData.maxLinearAcceleration;
	return _result;
}

SteeringOutput BlendSteering::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_currentWeight = 0.f;
	_result.angularVelocity = 0.f;
	_result.linearVelocity = { 0.f, 0.f };

	for (int i = 0; i < _behaviors.size(); i++) {
		_currentSteering = _behaviors[i].steeringBehaviour->Steering(behaviorData, enemy);
		_currentWeight = _behaviors[i].weight;
		_result.linearVelocity += (_currentSteering.linearVelocity * _currentWeight);
		_result.angularVelocity += _currentSteering.angularVelocity * _currentWeight;
	}
	return _result;
}

void BlendSteering::AddSteeringBehaviour(BehaviorAndWeight behaviour) {
	_behaviors.emplace_back(behaviour);
}

void BlendSteering::ClearBehaviours() {
	_behaviors.clear();
}

SteeringOutput PrioritySteering::Steering(BehaviorData behaviorData, EnemyBase& enemy) {
	_result.linearVelocity = { 0.f, 0.f };
	_result.angularVelocity = 0.f;

	for (unsigned int i = 0; i < _groups.size(); i++) {
		_result = _groups[i].Steering(behaviorData, enemy);
		if (_result.linearVelocity.absolute() > FLT_EPSILON || abs(_result.angularVelocity) > FLT_EPSILON) {
			return _result;
		}
	}
	return _result;
}

void PrioritySteering::AddGroup(BlendSteering& behaviour) {
	_groups.emplace_back(behaviour);
	behaviour.ClearBehaviours();
}
