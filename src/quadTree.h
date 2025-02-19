#pragma once
#include "collision.h"
#include "vector2.h"

#include <array>
#include <vector>

#include "debugDrawer.h"
#include "gameEngine.h"

struct QuadTreeNode {
	AABB rectangle;

	bool Contains(std::shared_ptr<Collider> collider);
	bool Intersect(std::shared_ptr<Collider> range);
};

template<typename T> 
class QuadTree {
public:
	QuadTree(QuadTreeNode boundary, unsigned int capacity);
	~QuadTree();

	bool Insert(T object, std::shared_ptr<Collider> collider);

	std::vector<T> Query(std::shared_ptr<Collider> range);

	void Clear();

	void Subdevide();

	void Render();

private:
	QuadTreeNode _upperLeft;
	QuadTreeNode _upperRight;
	QuadTreeNode _lowerLeft;
	QuadTreeNode _lowerRight;

	bool _divided = false;

	unsigned int _capacity = 0;
	QuadTreeNode _quadTreeNode;

	std::array<std::shared_ptr<QuadTree<T>>, 4> _quadTreeChildren;
	
	std::vector<T> _objectsFound;
	std::vector<T> _objectsFoundInChild;
	std::vector<T> _objectsInserted;

	std::vector<std::shared_ptr<Collider>> _colliders;
};
template<typename T>
inline QuadTree<T>::QuadTree(QuadTreeNode boundary, unsigned int capacity) {
	_quadTreeNode = boundary;
	_capacity = capacity;

	_quadTreeChildren[0] = nullptr;
	_quadTreeChildren[1] = nullptr;
	_quadTreeChildren[2] = nullptr;
	_quadTreeChildren[3] = nullptr;
}
template<typename T>
inline QuadTree<T>::~QuadTree() {
	Clear();
}

//Inserts an object into the quadtree node if the circle collider is in that node
template<typename T>
inline bool QuadTree<T>::Insert(T object, std::shared_ptr<Collider> collider) {
	if (!_quadTreeNode.Contains(collider)) {
		return false;
	}
	//If the node is at its max capacity it will subdevide into 4 nodes
	if (_objectsInserted.size() < _capacity) {
		_objectsInserted.emplace_back(object);
		_colliders.emplace_back(collider);
		return true;
	} else {
		if (!_divided) {
			Subdevide();
		}
		//After subdividing it checks if it can insert the object in one of the children nodes
		if (_quadTreeChildren[0]->Insert(object, collider)) {
			return true;
		} else if (_quadTreeChildren[1]->Insert(object, collider)) {
			return true;
		} else if (_quadTreeChildren[2]->Insert(object, collider)) {
			return true;
		} else if (_quadTreeChildren[3]->Insert(object, collider)) {
			return true;
		}
	}
}
//Returns a vector of the objects the collider hit
template<typename T>
inline std::vector<T> QuadTree<T>::Query(std::shared_ptr<Collider> range) {
	_objectsFound.clear();
	//Checks if the collider is inside the quadtree node
	if (_quadTreeNode.Intersect(range)) {
		/*Loops through all the items in the node, checks if they are interacting with the collider.
		If it does, add it to the vector*/
		for (unsigned int i = 0; i < _objectsInserted.size(); i++) {
			if (ColliderIntersect(range, _colliders[i])) {
				_objectsFound.emplace_back(_objectsInserted[i]);
			}
		}
		/*If the node has divided, the query function is called from every child
		Then I insert the queried objects from all the children into the objectsFound vector*/
		if (_divided) {
			_objectsFoundInChild.clear();
			if (_quadTreeChildren[0]) {
				_objectsFoundInChild = _quadTreeChildren[0]->Query(range);
				_objectsFound.insert(_objectsFound.end(), _objectsFoundInChild.begin(), _objectsFoundInChild.end());
			}
			if (_quadTreeChildren[1]) {
				_objectsFoundInChild = _quadTreeChildren[1]->Query(range);
				_objectsFound.insert(_objectsFound.end(), _objectsFoundInChild.begin(), _objectsFoundInChild.end());
			}
			if (_quadTreeChildren[2]) {
				_objectsFoundInChild = _quadTreeChildren[2]->Query(range);
				_objectsFound.insert(_objectsFound.end(), _objectsFoundInChild.begin(), _objectsFoundInChild.end());
			}
			if (_quadTreeChildren[3]) {
				_objectsFoundInChild = _quadTreeChildren[3]->Query(range);
				_objectsFound.insert(_objectsFound.end(), _objectsFoundInChild.begin(), _objectsFoundInChild.end());
			}
		}
	}
	return _objectsFound;
}
template<typename T>
inline void QuadTree<T>::Clear() {
	_objectsInserted.clear();
	_colliders.clear();
	_quadTreeChildren[0] = nullptr;
	_quadTreeChildren[1] = nullptr;
	_quadTreeChildren[2] = nullptr;
	_quadTreeChildren[3] = nullptr;
	_divided = false;
}
/*When deviding, I create 4 children that is a quarter of the size of the current node
and sets devided to true to prevent the node from calling Subdevide more than once*/
template<typename T>
inline void QuadTree<T>::Subdevide() {
	_upperLeft.rectangle.Init(Vector2<float>(
		_quadTreeNode.rectangle.GetPosition().x - (_quadTreeNode.rectangle.GetWidth() * 0.25f),
		_quadTreeNode.rectangle.GetPosition().y - (_quadTreeNode.rectangle.GetHeight() * 0.25f)),
		_quadTreeNode.rectangle.GetHeight() * 0.5f, _quadTreeNode.rectangle.GetWidth() * 0.5f);
	_quadTreeChildren[0] = std::make_shared<QuadTree<T>>(_upperLeft, _capacity);

	_upperRight.rectangle.Init(Vector2<float>(
		_quadTreeNode.rectangle.GetPosition().x + (_quadTreeNode.rectangle.GetWidth() * 0.25f),
		_quadTreeNode.rectangle.GetPosition().y - (_quadTreeNode.rectangle.GetHeight() * 0.25f)),
		_quadTreeNode.rectangle.GetHeight() * 0.5f, _quadTreeNode.rectangle.GetWidth() * 0.5f);
	_quadTreeChildren[1] = std::make_shared<QuadTree<T>>(_upperRight, _capacity);

	_lowerLeft.rectangle.Init(Vector2<float>(
		_quadTreeNode.rectangle.GetPosition().x - (_quadTreeNode.rectangle.GetWidth() * 0.25f),
		_quadTreeNode.rectangle.GetPosition().y + (_quadTreeNode.rectangle.GetHeight() * 0.25f)),
		_quadTreeNode.rectangle.GetHeight() * 0.5f, _quadTreeNode.rectangle.GetWidth() * 0.5f);
	_quadTreeChildren[2] = std::make_shared<QuadTree<T>>(_lowerLeft, _capacity);

	_lowerRight.rectangle.Init(Vector2<float>(
		_quadTreeNode.rectangle.GetPosition().x + (_quadTreeNode.rectangle.GetWidth() * 0.25f),
		_quadTreeNode.rectangle.GetPosition().y + (_quadTreeNode.rectangle.GetHeight() * 0.25f)),
		_quadTreeNode.rectangle.GetHeight() * 0.5f, _quadTreeNode.rectangle.GetWidth() * 0.5f);
	_quadTreeChildren[3] = std::make_shared<QuadTree<T>>(_lowerRight, _capacity);
	_divided = true;
}
template<typename T>
inline void QuadTree<T>::Render() {
	debugDrawer->AddDebugRectangle(_quadTreeNode.rectangle.position, _quadTreeNode.rectangle._min, 
		_quadTreeNode.rectangle._max, { 255, 125, 0, 255});

	for (unsigned int i = 0; i < _quadTreeChildren.size(); i++) {
		if (_quadTreeChildren[i]) {
			_quadTreeChildren[i]->Render();
		}
	}
}