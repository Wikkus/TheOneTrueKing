#include "objectBase.h"

#include "collision.h"
#include "gameEngine.h"

const unsigned int ObjectBase::GetObjectID() const {
    return _objectID;
}

const std::shared_ptr<Collider> ObjectBase::GetCollider() const {
    return std::shared_ptr<Collider>();
}

const float ObjectBase::GetOrientation() const {
    return _orientation;
}

const ObjectType ObjectBase::GetObjectType() const {
    return _objectType;
}

const std::shared_ptr<Sprite> ObjectBase::GetSprite() const {
    return _sprite;
}

const Vector2<float> ObjectBase::GetPosition() const {
    return _position;
}

const Vector2<float> ObjectBase::GetVelocity() const {
    return _velocity;
}

void ObjectBase::SetOrientation(float orientation) {
    _orientation = orientation;
}

void ObjectBase::SetPosition(Vector2<float> position) {
    _position = position;
}

void ObjectBase::SetVelocity(Vector2<float> velocity) {
    _velocity = velocity;
}
