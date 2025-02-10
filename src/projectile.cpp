#include "projectile.h"

#include "debugDrawer.h"
#include "enemyBoar.h"
#include "enemyManager.h"
#include "gameEngine.h"

Projectile::Projectile(ProjectileType projectileType, const char* spritePath, unsigned int objectID) :
	ObjectBase(objectID, ObjectType::Projectile) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(spritePath);
	_projectileType = projectileType;
	_circleCollider = std::make_shared<Circle>();
	_circleCollider->Init(_position, 16.f);
}

Projectile::~Projectile() {}

void Projectile::Init() {}

void Projectile::Update() {
	_position += _velocity * deltaTime;
	_velocity = _direction * _projectileSpeed;
	_circleCollider->SetPosition(_position + _direction * (_sprite->h * 0.25f));
}

void Projectile::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}


const std::shared_ptr<Collider> Projectile::GetCollider() const {
	return _circleCollider;
}

const ProjectileType Projectile::GetProjectileType() const {
	return _projectileType;
}

const unsigned int Projectile::GetProjectileDamage() const {
	return _projectileDamage;
}

void Projectile::SetDirection(Vector2<float> direction) {
	_direction = direction;
}

void Projectile::SetPosition(Vector2<float> position) {
	_position = position;
	_circleCollider->SetPosition(_position + _direction * (_sprite->h * 0.25f));
}

void Projectile::ActivateProjectile(float orientation, unsigned int projectileDamage, unsigned int projectileSpeed, Vector2<float> direction, Vector2<float> position) {
	_orientation = orientation;
	_projectileDamage = projectileDamage;
	_projectileSpeed = projectileSpeed;

	_direction = direction.normalized();
	_velocity = _direction * _projectileSpeed;

	_position = position;
	_circleCollider->SetPosition(_position + _direction * (_sprite->h * 0.25f));
}

void Projectile::DeactivateProjectile() {
	_orientation = 0.f;
	_direction = { 0.f, 0.f };
	_velocity = { 0.f, 0.f };
	_position = { -10000.f, 10000.f };
	_circleCollider->SetPosition(_position);
}
