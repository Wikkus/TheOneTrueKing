#include "managerBase.h"
#include "objectBase.h"
#include "quadTree.h"

ManagerBase::ManagerBase() {}

void ManagerBase::InsertObjectsQuadtree() {
	for (auto& object : _activeObjects) {
		if (!object.second->GetCollider()->GetIsActive()) {
			continue;
		}
		objectBaseQuadTree->Insert(object.second, object.second->GetCollider());
	}
}

void ManagerBase::Reset() {
	RemoveAllObjects();
}
