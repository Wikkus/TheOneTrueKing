#include <vector>

template<typename T>
class ObjectPool {
public:
	ObjectPool(unsigned int numberOfObjects);
	~ObjectPool(){}

	bool IsEmpty();

	unsigned int PoolSize();
	void PoolObject(T object);

	T SpawnObject();

private:
	std::vector<T> _objectPool;

	T _spawnedObject;
};

template<typename T>
inline ObjectPool<T>::ObjectPool(unsigned int numberOfObjects) {
	_objectPool.reserve(numberOfObjects);
}

template<typename T>
inline bool ObjectPool<T>::IsEmpty() {
	return _objectPool.empty();
}

template<typename T>
inline unsigned int ObjectPool<T>::PoolSize() {
	return _objectPool.size();
}

template<typename T>
inline void ObjectPool<T>::PoolObject(T object) {
	_objectPool.emplace_back(object);
}

template<typename T>
inline T ObjectPool<T>::SpawnObject() {
	if (!IsEmpty()) {
		_spawnedObject = _objectPool.back();
		_objectPool.pop_back();
		return _spawnedObject;
	}
}
