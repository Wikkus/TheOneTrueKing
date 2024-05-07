#include "projectile.h"

#include "debugDrawer.h"
#include "enemyBoar.h"
#include "enemyManager.h"
#include "gameEngine.h"

Projectile::Projectile(ProjectileType projectileType, const char* spritePath, unsigned int projectileDamage, unsigned int objectID) : 
	ObjectBase(objectID, ObjectType::Projectile) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(spritePath);
	_projectileType = projectileType;
	_projectileDamage = projectileDamage;

	_circleCollider.radius = 8.f;
	_circleCollider.position = _position;
}

Projectile::~Projectile() {}

void Projectile::Init() {}

void Projectile::Update() {
	_position += _direction * _projectileSpeed * deltaTime;
	_circleCollider.position = _position + _direction * (_sprite->h * 0.25f);
}

void Projectile::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

void Projectile::RenderText() {}

const Circle Projectile::GetCollider() const {
	return _circleCollider;
}

const ProjectileType Projectile::GetProjectileType() const {
	return _projectileType;
}

const ObjectType Projectile::GetObjectType() const {
	return _objectType;
}

const unsigned int Projectile::GetObjectID() const {
	return _objectID;
}

const std::shared_ptr<Sprite> Projectile::GetSprite() const {
	return _sprite;
}

const float Projectile::GetOrientation() const {
	return _orientation;
}

const unsigned int Projectile::GetProjectileDamage() const {
	return _projectileDamage;
}

const Vector2<float> Projectile::GetPosition() const {
	return _position;
}

void Projectile::SetDirection(Vector2<float> direction) {
	_direction = direction;
}

void Projectile::SetTargetOrientation(float orientation) {
	_orientation = orientation;
}

void Projectile::SetTargetPosition(Vector2<float> position) {
	_position = position;
	_circleCollider.position = _position + _direction * (_sprite->h * 0.25f);
}

void Projectile::ActivateProjectile(float orientation, Vector2<float> direction, Vector2<float> position) {
	_orientation = orientation;
	_direction = direction.normalized();
	_position = position;
	_circleCollider.position = _position + _direction * (_sprite->h * 0.25f);
}

void Projectile::DeactivateProjectile() {
	_orientation = 0.f;
	_direction = Vector2<float>(0.f, 0.f);
	_position = Vector2<float>(-10000.f, 10000.f);
	_circleCollider.position = _position;
}
