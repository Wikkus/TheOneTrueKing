#pragma once
#include "managerBase.h"

class WeaponComponent;

template<typename T> class ObjectPool;

enum class WeaponType;

class WeaponManager : public ManagerBase {
public:
	WeaponManager();
	~WeaponManager() {}

	void Init() override;

	void RemoveAllObjects() override;
	void RemoveObject(const unsigned int& objectID) override;
	void Reset() override;

	std::shared_ptr<WeaponComponent> SpawnWeapon(const WeaponType& weaponTyp, std::shared_ptr<ObjectBase> owner);

	void CreateWeapon(const WeaponType& weaponType);

private:
	std::shared_ptr<WeaponComponent> CastAsWeapon(std::shared_ptr<ObjectBase> currentObject);
	std::shared_ptr<WeaponComponent> _currentWeapon = nullptr;
	std::unordered_map<WeaponType, std::shared_ptr<ObjectPool<std::shared_ptr<WeaponComponent>>>> _weaponPools;
	
	unsigned int _numberOfWeaponTypes = 0;
	unsigned int _weaponAmountLimit = 3000;
};

