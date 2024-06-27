#pragma once
#include "dataStructuresAndMethods.h"
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

std::vector<CostAndSlot> SortByCost(std::vector<CostAndSlot> slotsAndCosts);
std::vector<CharacterAndSlots> SortByAssignmentEase(std::vector<CharacterAndSlots> characterAndSlots);

class FormationPattern {
public:
	FormationPattern() {}
	~FormationPattern() {}

	virtual void CreateSlots(std::array<unsigned int, 2> spawnCountPerRow, std::shared_ptr<AnchorPoint> anchorPoint) = 0;

	virtual unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) = 0;

	virtual StaticCharacter GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) = 0;
	virtual StaticCharacter GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) = 0;

	virtual std::unordered_map<SlotAttackType, unsigned int> GetSlotsPerType() = 0;

	virtual float GetSlotCost(WeaponType weaponType, unsigned int index) = 0;

	virtual const unsigned int GetNumberOfSlots() const = 0;

	virtual bool SupportsSlots(unsigned int slotCount) = 0;
	
	virtual void SetNumberOfSlots(unsigned int numberOfSlots) = 0;

protected:
	unsigned int _numberOfSlots = 0;
	StaticCharacter _location;
	StaticCharacter _result;

	std::unordered_map<SlotAttackType, unsigned int> _amountSlotsPerType;
};

class DefensiveCirclePattern : public FormationPattern {
public:
	DefensiveCirclePattern();
	~DefensiveCirclePattern() {}
	void CreateSlots(std::array<unsigned int, 2> spawnCountPerRow, std::shared_ptr<AnchorPoint> anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	StaticCharacter GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) override;
	StaticCharacter GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) override;
	
	std::unordered_map<SlotAttackType, unsigned int> GetSlotsPerType() override;
	
	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetNumberOfSlots(unsigned int numberOfSlots) override;

private:
	void CreateSlotsOfType(std::shared_ptr<AnchorPoint> anchorPoint, unsigned int amountSlots, float radius,
		SlotAttackType attackType, bool holdOrientation);

	bool _lockOrientation = false;
	
	float _angleAroundCircle = 0.f;
	float _radius = 0.f;
	float _characterRadius = 16.f;

	unsigned int _filledSlots = 0;

	std::vector<SlotPositionAndType> _slotPositionAndType;
};

class VShapePattern : public FormationPattern {
public:
	VShapePattern();
	~VShapePattern() {}

	void CreateSlots(std::array<unsigned int, 2> spawnCountPerRow, std::shared_ptr<AnchorPoint> anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	StaticCharacter GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) override;
	StaticCharacter GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) override;
	
	std::unordered_map<SlotAttackType, unsigned int> GetSlotsPerType() override;

	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetNumberOfSlots(unsigned int numberOfSlots) override;

private:
	void CreateSlotsOfType(std::shared_ptr<AnchorPoint> anchorPoint, unsigned int amountSlots,
		Vector2<float> frontSlotPosition, SlotAttackType attackType);
	
	std::vector<SlotPositionAndType> _slotPositionAndType;
	float _offset = 15.f;
	float _rowDistance = 32.f;
	unsigned int _multiplier = 0;
	
	Vector2<float> _frontPosition = { 0.f, 0.f };

	Vector2<float> _positionOffset = { 0.f, 0.f };

	Vector2<float> _slotOffsetA = { -32.f, 32.f };
	Vector2<float> _slotOffsetB = { 32.f, 32.f };

};

class FormationManager {
public:
	FormationManager(FormationType formationType, std::array<unsigned int, 2> spawnCountPerRow, 
		std::shared_ptr<AnchorPoint> anchorPoint, bool posWithAnchorOri);
	~FormationManager() {}

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
	bool _posWithAnchorOri = false;

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

