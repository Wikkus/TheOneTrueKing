#include "quadTree.h"

bool QuadTreeNode::Contains(std::shared_ptr<Collider> circleCollider) {
	switch (circleCollider->GetColliderType()) {
	case ColliderType::AABB:
		return AABBIntersect(rectangle, *std::static_pointer_cast<AABB>(circleCollider));
		break;
	case ColliderType::Circle:
		return AABBCircleIntersect(rectangle, *std::static_pointer_cast<Circle>(circleCollider));
		break;
	default:
		break;
	}
	return false;
}

bool QuadTreeNode::Intersect(std::shared_ptr<Collider> range) {
	switch (range->GetColliderType()) {
	case ColliderType::AABB:
		return AABBIntersect(rectangle, *std::static_pointer_cast<AABB>(range));
		break;
	case ColliderType::Circle:
		return AABBCircleIntersect(rectangle, *std::static_pointer_cast<Circle>(range));
		break;
	default:
		break;
	}
	return false;
}
