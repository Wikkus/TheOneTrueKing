#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

class ObjectBase;

class ManagerBase {
public:
	ManagerBase();
	~ManagerBase() {}

	virtual void Init() {}
	virtual void Update() {}


	virtual void Render() {}
	virtual void RenderText() {}

	virtual void InsertObjectsQuadtree();
	virtual void RemoveAllObjects() {}
	virtual void RemoveObject(const unsigned int& objectID) {}
	virtual void Reset();

protected:
	std::unordered_map<unsigned int, std::shared_ptr<ObjectBase>> _activeObjects;
	std::vector<std::shared_ptr<ObjectBase>> _removeObjects;

	const unsigned int _objectAmountLimit = 3000;
};

