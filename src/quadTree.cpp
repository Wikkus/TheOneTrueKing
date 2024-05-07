#include "quadTree.h"

bool QuadTreeNode::Contains(Circle circleCollider) {
	return AABBCircleIntersect(rectangle, circleCollider);
}

bool QuadTreeNode::Intersect(Circle range) {
	return AABBCircleIntersect(rectangle, range);
}
