#include "enemyHuman.h"

#include "dataStructuresAndMethods.h"
#include "debugDrawer.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "quadTree.h"
#include "steeringBehavior.h"
#include "timerManager.h"
#include "weaponComponent.h"

#include <memory>

EnemyHuman::EnemyHuman(unsigned int objectID, EnemyType enemyType) :
	EnemyBase(objectID, enemyType) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_humanSprite);

	_position = Vector2<float>(-10000.f, -10000.f);

	_circleCollider.position = _position;
	_circleCollider.radius = 12.f;
	
	_maxHealth = 15;
	_currentHealth = _maxHealth;

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 2.5f;
	_behaviorData.maxRotation = PI * 2.f;
	
	_behaviorData.timeToTarget = 0.1f;
	
	_behaviorData.maxLinearAcceleration = 75.f;
	_behaviorData.maxSpeed = 75.f;
	_behaviorData.separationThreshold = _circleCollider.radius * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_behaviorData.characterRadius = _circleCollider.radius;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->ClearBehaviours();
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);
}

EnemyHuman::~EnemyHuman() {}

void EnemyHuman::Init() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();
	_direction = Vector2<float>(_behaviorData.targetPosition - _position).normalized();
	_orientation = VectorAsOrientation(_direction);
	PickWeapon();
}

void EnemyHuman::Update() {
	_queriedObjects = objectBaseQuadTree->Query(_circleCollider);
	SetTargetPosition(playerCharacter->GetPosition());
	UpdateMovement();
	
	HandleAttack();
	_circleCollider.position = _position;
}

void EnemyHuman::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
	_weaponComponent->Render(_position, _orientation);
}

void EnemyHuman::RenderText() {}

const Circle EnemyHuman::GetCollider() const {
	return _circleCollider;
}

const EnemyType EnemyHuman::GetEnemyType() const {
	return _enemyType;
}

const ObjectType EnemyHuman::GetObjectType() const {
	return _objectType;
}

const float EnemyHuman::GetOrientation() const {
	return _orientation;
}

const float EnemyHuman::GetRotation() const {
	return _rotation;
}

const int EnemyHuman::GetCurrentHealth() const {
	return _currentHealth;
}

const unsigned int EnemyHuman::GetObjectID() const {
	return _objectID;
}

const std::shared_ptr<Sprite> EnemyHuman::GetSprite() const {
	return _sprite;
}

const BehaviorData EnemyHuman::GetBehaviorData() const {
	return _behaviorData;
}

const Vector2<float> EnemyHuman::GetPosition() const {
	return _position;
}

const Vector2<float> EnemyHuman::GetVelocity() const {
	return _velocity;
}

const std::vector<std::shared_ptr<ObjectBase>> EnemyHuman::GetQueriedObjects() const {
	return _queriedObjects;
}

const std::shared_ptr<WeaponComponent> EnemyHuman::GetWeaponComponent() const {
	return _weaponComponent;
}

void EnemyHuman::ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position) {
	_orientation = orienation;
	_direction = direction;
	_position = position;
	_circleCollider.position = position;
	Init();
}

void EnemyHuman::DeactivateEnemy() {
	_orientation = 0.f;
	_direction = Vector2<float>(0.f, 0.f);
	_position = Vector2<float>(-10000.f, -10000.f);
	_circleCollider.position = _position;
}

void EnemyHuman::HandleAttack() {
	//Depending on the weapon, the attack works differently
	_weaponComponent->Attack(_position, _orientation);
}

void EnemyHuman::SetPosition(Vector2<float> position) {
	_position = position;
}

void EnemyHuman::SetTargetPosition(Vector2<float> targetPosition) {
	_behaviorData.targetPosition = targetPosition;
}

void EnemyHuman::SetTargetOrientation(float targetOrientation) {
	_behaviorData.targetOrientation = targetOrientation;
}

void EnemyHuman::SetVelocity(Vector2<float> velocity) {
	_velocity = velocity;
}

bool EnemyHuman::TakeDamage(unsigned int damageAmount) {
	_currentHealth -= damageAmount;
	if (_currentHealth <= 0) {	
		return true;
	}
	return false;
}

void EnemyHuman::UpdateMovement() {
	_position += _velocity * deltaTime;
	_orientation += _rotation * deltaTime;
	_steeringOutput = _prioritySteering->Steering(_behaviorData, *this);
	_rotation += _steeringOutput.angularVelocity * deltaTime;
	_velocity += _steeringOutput.linearVelocity * deltaTime;

	if (_velocity.absolute() > _behaviorData.maxSpeed) {
		_velocity.normalize();
		_velocity *= _behaviorData.maxSpeed;
	}
}

void EnemyHuman::PickWeapon() {
	/*Human enemy picks a weapon component at spawn
	to either fight in melee range or shoot fireballs*/
	std::uniform_int_distribution decideWeapon{ 0, 1 };
	int weaponPicked = decideWeapon(randomEngine);
	switch (weaponPicked) {
	case 0:
		_weaponComponent = std::make_shared<SwordComponent>();
		break;
	case 1:
		_weaponComponent = std::make_shared<StaffComponent>();
		break;
	default:
		_weaponComponent = std::make_shared<SwordComponent>();
		break;
	}
	std::uniform_real_distribution range{ _weaponComponent->GetAttackRange() * 0.5f, _weaponComponent->GetAttackRange() };
	float attackRange = range(randomEngine);

	_behaviorData.linearTargetRadius = attackRange;
	_behaviorData.linearSlowDownRadius = attackRange + 100.f;

}
