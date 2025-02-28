#include "projectile.h"

#include "debugDrawer.h"
#include "enemyBoar.h"
#include "enemyManager.h"
#include "gameEngine.h"

Projectile::Projectile(const ProjectileType& projectileType, const char* spritePath) : ObjectBase(ObjectType::Projectile) {
	_projectileType = projectileType;

	_sprite = std::make_shared<Sprite>();
	_sprite->Load(spritePath);
	_position = deactivatedPosition;

	_collider = std::make_shared<Circle>();
	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->GetWidth() * 0.5f);
}

Projectile::~Projectile() {}

void Projectile::Init() {}

void Projectile::Update() {
	_velocity = _direction * _speed;
	_position += _velocity * deltaTime;
	_collider->SetPosition(_position + _direction * (_sprite->GetHeight() * 0.25f));
}

void Projectile::Render() {
	_sprite->RenderWithOrientation(0, _position, _orientation);
}

const std::shared_ptr<Collider> Projectile::GetCollider() const {
	return _collider;
}

const std::shared_ptr<ObjectBase> Projectile::GetOwner() const {
	return _owner;
}

const ProjectileType Projectile::GetProjectileType() const {
	return _projectileType;
}

const int Projectile::GetDamage() const {
	return _damage;
}

void Projectile::SetDirection(const Vector2<float>& direction) {
	_direction = direction;
}

void Projectile::SetPosition(const Vector2<float>& position) {
	_position = position;
	_collider->SetPosition(_position + _direction * (_sprite->GetWidth() * 0.25f));
}

void Projectile::ActivateProjectile(std::shared_ptr<ObjectBase> owner, const float& orientation, const Vector2<float>& direction,
	const Vector2<float>& position, const int& damage, const float& speed) {
	_owner = owner;
	
	_orientation = orientation;

	_direction = direction.normalized();
	_velocity = _direction * _speed;

	_position = position;
	_collider->SetPosition(_position + _direction * (_sprite->GetWidth() * 0.25f));
	
	_damage = damage;
	_speed = speed;
}

void Projectile::DeactivateObject() {
	_owner = nullptr;
	_position = deactivatedPosition;
	_collider->SetPosition(_position);
}
