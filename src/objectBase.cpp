#include "objectBase.h"

#include "collision.h"
#include "gameEngine.h"
#include "quadTree.h"

ObjectBase::ObjectBase(const ObjectType& objectType) : _objectID(lastObjectID), _objectType(objectType) {
    lastObjectID++;
}

const unsigned int ObjectBase::GetObjectID() const {
    return _objectID;
}

const std::shared_ptr<Collider> ObjectBase::GetCollider() const {
    return _collider;
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

const std::shared_ptr<ObjectBase> ObjectBase::GetTargetObject() const {
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

const int ObjectBase::GetDamage() const {
    return 0;
}

void ObjectBase::SetOrientation(const float& orientation) {
    _orientation = orientation;
}

void ObjectBase::SetRotation(const float& rotation) {
    _rotation = rotation;
}

void ObjectBase::SetPosition(const Vector2<float>& position) {
    _position = position;
}

void ObjectBase::SetTargetPosition(const Vector2<float>& targetPosition) {
    _targetPosition = targetPosition;
}

void ObjectBase::SetVelocity(const Vector2<float>& velocity) {
    _velocity = velocity;
}

void ObjectBase::QueryObjects() {
    _queriedObjects = objectBaseQuadTree->Query(_collider);

    _queriedProjectiles = projectileQuadTree->Query(_collider);
    _queriedObjects.insert(_queriedObjects.end(), _queriedProjectiles.begin(), _queriedProjectiles.end());

    _queriedObstacles = obstacleQuadTree->Query(_collider);
    _queriedObjects.insert(_queriedObjects.end(), _queriedObstacles.begin(), _queriedObstacles.end());
}

void ObjectBase::TakeDamage(const int& damageAmount) {}

void ObjectBase::ActivateObject(const Vector2<float>& position, const Vector2<float>& direction, const float& orienation) {
    _position = position;
    _direction = direction;
    _orientation = orienation;
}

void ObjectBase::DeactivateObject() {
    _position = deactivatedPosition;
    _collider->SetPosition(_position);
}
