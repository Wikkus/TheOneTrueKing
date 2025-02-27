#pragma once
#include "vector2.h"
#include "weaponComponent.h"

#include <array>
#include <climits>
#include <memory>
#include <unordered_map>
#include <vector>

class EnemyBase;

enum class FormationType {
	DefensiveCircle,
	VShape,
	Count
};

enum class SlotAttackType {
	Defender,
	Mage,
	Swordsman,
	Count
};

struct AnchorPoint {
	Vector2<float> position = { 0.f , 0.f };
	Vector2<float> targetPosition = { 0.f , 0.f };

	Vector2<float> direction = { 0.f , 0.f };

	float distance = 0.f;
	float movementSpeed = 60.f;
	float orientation = 0.f;
};

struct CostAndSlot {
	float cost = 0.f;
	unsigned int slotNumber = 0;
};

struct SlotPositionAndType {
	unsigned int slotNumber = 0;
	bool lockOrientation = false;
	float orientation = 0.f;
	Vector2<float> position;
	SlotAttackType slotAttackType;
};

struct CharacterAndSlots {
	bool isAssigned = false;
	std::shared_ptr<EnemyBase> enemyCharacter = nullptr;
	float assignmentEase = 0.f;
	std::vector<CostAndSlot> costAndSlots;
};

struct SlotAssignment {
	bool isOnScreen = false;
	std::shared_ptr<EnemyBase> enemyCharacter;
	unsigned int slotNumber = INT_MAX;
};

struct StaticCharacter {
	Vector2<float> position = { 0.f , 0.f };
	float orientation = 0.f;
};

std::vector<CostAndSlot> SortByCost(std::vector<CostAndSlot> slotsAndCosts);
std::vector<CharacterAndSlots> SortByAssignmentEase(std::vector<CharacterAndSlots> characterAndSlots);

class FormationPattern {
public:
	FormationPattern() {}
	~FormationPattern() {}

	virtual void CreateSlots(const std::array<unsigned int, 2>& spawnCountPerRow, const std::shared_ptr<AnchorPoint>& anchorPoint) {}

	StaticCharacter GetDriftOffset(const AnchorPoint& anchorPoint, const std::vector<SlotAssignment>& slotAssignments);
	StaticCharacter GetSlotLocation(const AnchorPoint& anchorPoint, const unsigned int& slotNumber, const unsigned int& numberOfSlots);

	std::unordered_map<SlotAttackType, unsigned int> GetSlotsPerType() const;

	virtual const float GetSlotCost(const WeaponType& weaponType, const unsigned int& index) const;

	virtual const unsigned int GetNumberOfSlots() const;

	virtual bool SupportsSlots(const unsigned int& slotCount);
	
	virtual void SetNumberOfSlots(const unsigned int& numberOfSlots);

protected:
	unsigned int _numberOfSlots = 0;

	StaticCharacter _location;
	StaticCharacter _result;

	StaticCharacter _currentLocation;
	Vector2<float> _currentPosition;

	unsigned int _amountSlots = 0;
	
	std::unordered_map<SlotAttackType, unsigned int> _amountSlotsPerType;
	std::vector<SlotPositionAndType> _slotPositionAndType;
};

class DefensiveCirclePattern : public FormationPattern {
public:
	DefensiveCirclePattern() {}
	~DefensiveCirclePattern() {}
	void CreateSlots(const std::array<unsigned int, 2>& spawnCountPerRow, const std::shared_ptr<AnchorPoint>& anchorPoint) override;

private:
	void CreateSlotsOfType(const std::shared_ptr<AnchorPoint>& anchorPoint, const unsigned int& amountSlots, const float& radius,
		const SlotAttackType& attackType, const bool& lockOrientation);
	
	float _angleAroundCircle = 0.f;
	float _radius = 0.f;
	float _characterRadius = 16.f;

	unsigned int _filledSlots = 0;

};

class VShapePattern : public FormationPattern {
public:
	VShapePattern() {}
	~VShapePattern() {}

	void CreateSlots(const std::array<unsigned int, 2>& spawnCountPerRow, const std::shared_ptr<AnchorPoint>& anchorPoint) override;

private:
	void CreateSlotsOfType(const std::shared_ptr<AnchorPoint>& anchorPoint, const unsigned int& amountSlots,
		const Vector2<float>& frontSlotPosition, const SlotAttackType& attackType, const bool& lockOrientation);

	float _offset = 15.f;
	float _rowDistance = 32.f;
	unsigned int _multiplier = 0;
	
	Vector2<float> _frontPosition = { 0.f, 0.f };

	Vector2<float> _positionOffset = { 0.f, 0.f };

	Vector2<float> _slotOffsetA = { -32.f, 32.f };
	Vector2<float> _slotOffsetB = { 32.f, 32.f };

};

class FormationHandler {
public:
	FormationHandler(const FormationType& formationType, const std::array<unsigned int, 2>& spawnCountPerRow, 
		const std::shared_ptr<AnchorPoint>& anchorPoint, const bool& gotOrientation);
	~FormationHandler() {}

	bool AddCharacter(std::shared_ptr<EnemyBase> enemyCharacter);
	void UpdateAnchorPoint();
	void UpdateSlots();
	void ReconstructSlotAssignments();
	void RemoveCharacter(std::shared_ptr<EnemyBase> enemyCharacter);

	std::vector<SlotAssignment> GetSlotAssignments();
	std::shared_ptr<FormationPattern> GetFormationPattern();

	const bool GetInPosition() const;
	const unsigned int GetNumberOfSlots() const;

private:
	std::shared_ptr<AnchorPoint> _anchorPoint;
	std::shared_ptr<ObjectBase> _targetObject;
	StaticCharacter _driftOffset;

	StaticCharacter _location;
	StaticCharacter _slot;

	CharacterAndSlots _datum;
	CostAndSlot _slotDatum;

	SlotAssignment _slotAssignment;

	std::vector<bool> _filledSlots;
	std::vector<CharacterAndSlots> _characterAndSlots;
	std::shared_ptr<FormationPattern> _formationPattern;
	std::vector<SlotAssignment> _slotAssignments;
	
	bool _allSlotsOnScreen = false;
	bool _inPosition = false;
	bool _gotOrientation = false;

	unsigned int _slotsOnScreen = 0;
		
	float _anchorDistance = 0.f;
	float _costLimit = 2000.f;
	float _orientation = 0.f;
	float _slotCost = 0.f;

	unsigned int _numberOfSlots = 0;
	unsigned int _occupiedSlots = 0;
	unsigned int _slotNumber = 0;

	Vector2<float> _anchorDirection;
};

