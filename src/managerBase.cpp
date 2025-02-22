#include "managerBase.h"
#include "objectBase.h"
#include "quadTree.h"

void ManagerBase::InsertObjectsQuadtree() {
	for (unsigned int i = 0; i < _activeObjects.size(); i++) {
		objectBaseQuadTree->Insert(_activeObjects[i], _activeObjects[i]->GetCollider());
	}
}

void ManagerBase::TakeDamage(unsigned int objectIndex, int damageAmount) {
	_activeObjects[objectIndex]->TakeDamage(damageAmount);
}

int ManagerBase::Partition(std::vector<int>& objectVector, int start, int end) {
	_pivot = objectVector[start];
	_count = 0;

	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i] <= _pivot) {
			_count++;
		}
	}
	_pivotIndex = start + _count;

	std::swap(objectVector[_pivotIndex], objectVector[start]);
	std::swap(_activeObjects[_pivotIndex], _activeObjects[start]);
	_i = start;
	_k = end;

	while (_i < _pivotIndex && _k > _pivotIndex) {

		while (objectVector[_i] <= _pivot) {
			_i++;
		}
		while (objectVector[_k] > _pivot) {
			_k--;
		}
		if (_i < _pivotIndex && _k > _pivotIndex) {

			std::swap(objectVector[_i], objectVector[_k]);
			std::swap(_activeObjects[_i], _activeObjects[_k]);
			_i++;
			_k--;
		}
	}
	return _pivotIndex;
}

void ManagerBase::QuickSort(std::vector<int>& objectVector, int start, int end) {
	if (start >= end) {
		return;
	}
	_partition = Partition(objectVector, start, end);

	QuickSort(objectVector, start, _partition - 1);
	QuickSort(objectVector, _partition + 1, end);
}
