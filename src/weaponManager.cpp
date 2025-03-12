#include "weaponManager.h"

#include "objectBase.h"
#include "objectPool.h"
#include "quadTree.h"
#include "weaponComponent.h"

WeaponManager::WeaponManager() {
	_numberOfWeaponTypes = (unsigned int)WeaponType::Count;
	for (unsigned int i = 0; i < _numberOfWeaponTypes; i++) {
		_weaponPools[(WeaponType)i] = std::make_shared<ObjectPool<std::shared_ptr<WeaponComponent>>>(_weaponAmountLimit);
	}
}

void WeaponManager::Init() {
	for (unsigned i = 0; i < _weaponAmountLimit / _numberOfWeaponTypes; i++) {
		for (unsigned int k = 0; k < _numberOfWeaponTypes; k++) {
			CreateWeapon((WeaponType)k);
		}
	}
}

void WeaponManager::Update() {
	for (auto& obstacle : _activeObjects) {
		obstacle.second->Update();
	}
}

std::shared_ptr<WeaponComponent> WeaponManager::SpawnWeapon(const WeaponType& weaponType, std::shared_ptr<ObjectBase> owner) {
	if (_weaponPools[weaponType]->IsEmpty()) {
		CreateWeapon(weaponType);
	}
	_currentWeapon = _weaponPools[weaponType]->SpawnObject();
	_currentWeapon->SetOwner(owner);
	_currentWeapon->SetValuesToDefault();
	_activeObjects.insert(std::make_pair(_currentWeapon->GetObjectID(), _currentWeapon));
	return _currentWeapon;
}

void WeaponManager::CreateWeapon(const WeaponType& weaponType) {
	switch (weaponType) {
	case WeaponType::Shield:
		_weaponPools[weaponType]->PoolObject(std::make_shared<ShieldComponent>());
		break;
	case WeaponType::Staff:
		_weaponPools[weaponType]->PoolObject(std::make_shared<StaffComponent>());
		break;
	case WeaponType::SuperStaff:
		_weaponPools[weaponType]->PoolObject(std::make_shared<SuperStaffComponent>());
		break;
	case WeaponType::Sword:
		_weaponPools[weaponType]->PoolObject(std::make_shared<SwordComponent>());
		break;
	case WeaponType::Tusks:
		_weaponPools[weaponType]->PoolObject(std::make_shared<TusksComponent>());
		break;
	case WeaponType::Warstomp:
		_weaponPools[weaponType]->PoolObject(std::make_shared<WarstompComponent>());
		break;
	default:
		break;
	}
}

void WeaponManager::InsertObjectsQuadtree() {
	for (auto& object : _activeObjects) {
		if (!object.second->GetCollider()->GetIsActive()) {
			continue;
		}
		objectBaseQuadTree->Insert(object.second, object.second->GetCollider());
	}
}

void WeaponManager::RemoveAllObjects() {
	for (auto& weapon : _activeObjects) {
		_currentWeapon = CastAsWeapon(weapon.second);
		_currentWeapon->DeactivateObject();
		_weaponPools[_currentWeapon->GetWeaponType()]->PoolObject(_currentWeapon);
	}
	_activeObjects.clear();
	_currentWeapon = nullptr;
}

void WeaponManager::RemoveObject(const unsigned int& objectID) {
	if (_activeObjects.empty()) {
		return;
	}
	_currentWeapon = CastAsWeapon(_activeObjects[objectID]);
	_currentWeapon->DeactivateObject();
	_weaponPools[_currentWeapon->GetWeaponType()]->PoolObject(_currentWeapon);
	_activeObjects.erase(objectID);
	_currentWeapon = nullptr;
}

void WeaponManager::Reset() {
	RemoveAllObjects();
}

std::shared_ptr<WeaponComponent> WeaponManager::CastAsWeapon(std::shared_ptr<ObjectBase> currentObject) {
	return std::static_pointer_cast<WeaponComponent>(currentObject);
}
