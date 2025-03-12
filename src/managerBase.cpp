#include "managerBase.h"
#include "objectBase.h"
#include "quadTree.h"

ManagerBase::ManagerBase() {}

void ManagerBase::Update() {
	for (auto& obstacle : _activeObjects) {
		obstacle.second->Update();
		obstacle.second->QueryObjects();
	}
}

void ManagerBase::Render() {
	for (auto& objectBase : _activeObjects) {
		objectBase.second->Render();
	}
}

void ManagerBase::RenderText() {
	for (auto& objectBase : _activeObjects) {
		objectBase.second->RenderText();
	}
}

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
