#include "objectBase.h"

#include "collision.h"
#include "gameEngine.h"
#include "quadTree.h"

const unsigned int ObjectBase::GetObjectID() const {
    return _objectID;
}

const std::shared_ptr<Collider> ObjectBase::GetCollider() const {
    return std::shared_ptr<Collider>();
}

const float ObjectBase::GetOrientation() const {
    return _orientation;
}

const float ObjectBase::GetRotation() const {
    return _rotation;
}

const ObjectType ObjectBase::GetObjectType() const {
    return _objectType;
}

const std::vector<std::shared_ptr<ObjectBase>> ObjectBase::GetQueriedObjects() const {
    return _queriedObjects;
}

const std::shared_ptr<ObjectBase> ObjectBase::GetCurrentTarget() const {
    return _currentTarget;
}

const std::shared_ptr<Sprite> ObjectBase::GetSprite() const {
    return _sprite;
}


const Vector2<float> ObjectBase::GetDirection() const {
    return _direction;
}

const Vector2<float> ObjectBase::GetPosition() const {
    return _position;
}

const Vector2<float> ObjectBase::GetTargetPosition() const {
    return _targetPosition;
}

const Vector2<float> ObjectBase::GetVelocity() const {
    return _velocity;
}

void ObjectBase::SetOrientation(float orientation) {
    _orientation = orientation;
}

void ObjectBase::SetRotation(float rotation) {
    _rotation = rotation;
}

void ObjectBase::SetPosition(Vector2<float> position) {
    _position = position;
}

void ObjectBase::SetTargetPosition(Vector2<float> targetPosition) {
    _targetPosition = targetPosition;
}

void ObjectBase::SetVelocity(Vector2<float> velocity) {
    _velocity = velocity;
}

void ObjectBase::QueryObjects() {
    _queriedObjects = objectBaseQuadTree->Query(_collider);
}

void ObjectBase::TakeDamage(unsigned int damageAmount) {}