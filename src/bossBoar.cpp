#include "bossBoar.h"
#include "playerCharacter.h"

#include "decisionTree.h"

BoarBoss::BoarBoss(unsigned int objectID, EnemyType enemyType) : EnemyBase(objectID, enemyType) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_bossBoarSprite);

	_position = Vector2<float>(-10000.f, -10000.f);

	_circleCollider = std::make_shared<Circle>();
	_circleCollider->Init(_position, 64.f);

	_behaviorData.characterRadius = _circleCollider->GetRadius();

	_maxHealth = 1500;
	_currentHealth = _maxHealth;

	_attackDamage = 300;
	_attackRange = 450;
	_attackRadius = std::make_shared<Circle>();
	_attackRadius->Init(_position, 300.f);

	_behaviorData.angularSlowDownRadius = PI * 0.5f;
	_behaviorData.angularTargetRadius = PI * 0.005f;
	_behaviorData.maxAngularAcceleration = PI * 1.5f;
	_behaviorData.maxRotation = PI * 2.f;

	_behaviorData.timeToTarget = 0.1f;

	_behaviorData.maxLinearAcceleration = 50.f;
	_behaviorData.maxSpeed = 100.f;

	_behaviorData.linearTargetRadius = _attackRange - 5.f;
	_behaviorData.linearSlowDownRadius = _attackRange + 25.f;

	_behaviorData.separationThreshold = _circleCollider->GetRadius() * 1.5f;
	_behaviorData.decayCoefficient = 10000.f;

	_prioritySteering = std::make_shared<PrioritySteering>();
	_blendSteering = std::make_shared<BlendSteering>();

	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<SeparationBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<ArriveBehavior>(), 1.f));
	_blendSteering->AddSteeringBehaviour(BehaviorAndWeight(std::make_shared<FaceBehavior>(), 1.f));
	_prioritySteering->AddGroup(*_blendSteering);

	
}

BoarBoss::~BoarBoss() {}

void BoarBoss::Init() {
	_behaviorData.targetPosition = playerCharacter->GetPosition();


	_position = Vector2<float>(windowWidth * 0.6f, windowHeight * 0.5f);


	_direction = _behaviorData.targetPosition - _position;

	_behaviorData.velocity = _velocity;


	DecisionTree temp(*this);

	temp.CreateTree();
}

void BoarBoss::Update() {
	
}

void BoarBoss::Render() {
}

void BoarBoss::RenderText() {
}

bool BoarBoss::TakeDamage(unsigned int damageAmount) {
	return false;
}

const std::shared_ptr<Collider> BoarBoss::GetCollider() const {
	return std::shared_ptr<Collider>();
}

const EnemyType BoarBoss::GetEnemyType() const {
	return EnemyType();
}

const ObjectType BoarBoss::GetObjectType() const {
	return ObjectType();
}

const float BoarBoss::GetOrientation() const {
	return 0.0f;
}

const float BoarBoss::GetRotation() const {
	return 0.0f;
}

const int BoarBoss::GetCurrentHealth() const {
	return 0;
}

const unsigned int BoarBoss::GetObjectID() const {
	return 0;
}

const std::shared_ptr<Sprite> BoarBoss::GetSprite() const {
	return std::shared_ptr<Sprite>();
}

const BehaviorData BoarBoss::GetBehaviorData() const {
	return BehaviorData();
}

const Vector2<float> BoarBoss::GetPosition() const {
	return Vector2<float>();
}

const Vector2<float> BoarBoss::GetVelocity() const {
	return Vector2<float>();
}

const std::vector<std::shared_ptr<ObjectBase>> BoarBoss::GetQueriedObjects() const {
	return std::vector<std::shared_ptr<ObjectBase>>();
}

void BoarBoss::ActivateEnemy(float orienation, Vector2<float> direction, Vector2<float> position, WeaponType weaponType) {
}

void BoarBoss::DeactivateEnemy() {
}

void BoarBoss::HandleAttack() {
}

void BoarBoss::SetPosition(Vector2<float> position) {
}

void BoarBoss::SetTargetPosition(Vector2<float> targetPosition) {
}

void BoarBoss::SetTargetOrientation(float targetOrientation) {
}

void BoarBoss::SetVelocity(Vector2<float> velocity) {
}

void BoarBoss::UpdateMovement() {
}
