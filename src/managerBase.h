#pragma once
#include <memory>
#include <vector>

class ObjectBase;

class ManagerBase {
public:
	ManagerBase() {}
	~ManagerBase() {}

	virtual void Init() {}
	virtual void Update() {}


	virtual void Render() {}
	virtual void RenderText() {}

	virtual void InsertObjectsQuadtree();
	virtual void Reset() {}

	void TakeDamage(unsigned int objectIndex, int damageAmount);

protected:
	int Partition(std::vector<int>& objectVector, int start, int end);
	void QuickSort(std::vector<int>& objectVector, int start, int end);

	std::vector<std::shared_ptr<ObjectBase>> _activeObjects;
	std::vector<int> _objectIDs;

	//Quicksort variables
	int _pivot = 0;
	int _pivotIndex = 0;
	int _count = 0;
	int _i = 0;
	int _k = 0;
	int _partition = 0;

};

