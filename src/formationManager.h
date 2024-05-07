#pragma once
#include "dataStructuresAndMethods.h"
#include "vector2.h"
#include "weaponComponent.h"

#include <array>
#include <climits>
#include <memory>
#include <vector>

class EnemyBase;

enum class FormationType {
	DefensiveCircle,
	SlotRole,
	VShape,
	Count
};

enum class SlotAttackType {
	Bow,
	Magic,
	Melee,
	Count
};

struct CostAndSlot {
	float cost = 0.f;
	unsigned int slotNumber = 0;
};

struct SlotPositionAndType {
	unsigned int slotNumber = 0;
	AnchorPoint positionAndOrientation;
	SlotAttackType slotAttackType;
};

struct CharacterAndSlots {
	bool isAssigned = false;
	std::shared_ptr<EnemyBase> enemyCharacter = nullptr;
	float assignmentEase = 0.f;
	std::vector<CostAndSlot> costAndSlots;
};

struct SlotAssignment {
	std::shared_ptr<EnemyBase> enemyCharacter;
	unsigned int slotNumber = INT_MAX;
};

std::vector<CostAndSlot> SortByCost(std::vector<CostAndSlot> slotsAndCosts);
std::vector<CharacterAndSlots> SortByAssignmentEase(std::vector<CharacterAndSlots> characterAndSlots);

class FormationPattern {
public:
	FormationPattern() {}
	~FormationPattern() {}

	virtual void CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) = 0;

	virtual unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) = 0;

	virtual AnchorPoint GetDriftOffset(std::vector<SlotAssignment> slotAssignments) = 0;
	virtual AnchorPoint GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) = 0;

	virtual float GetSlotCost(WeaponType weaponType, unsigned int index) = 0;

	virtual const unsigned int GetNumberOfSlots() const = 0;

	virtual bool SupportsSlots(unsigned int slotCount) = 0;

	virtual void SetSlotPositionAndType() = 0;
};

class DefensiveCirclePattern : public FormationPattern {
public:
	DefensiveCirclePattern() {}
	~DefensiveCirclePattern() {}
	void CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	AnchorPoint GetDriftOffset(std::vector<SlotAssignment> slotAssignments) override;
	AnchorPoint GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) override;
	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetSlotPositionAndType() override;

private:
	unsigned int _numberOfSlots = 9;
	float _characterRadius = 25.f;
};

class SlotRolePattern : public FormationPattern {
public:
	SlotRolePattern();
	~SlotRolePattern() {}
	void CreateSlots(unsigned int maxAmountSlots, AnchorPoint anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	AnchorPoint GetDriftOffset(std::vector<SlotAssignment> slotAssignments) override;
	AnchorPoint GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) override;

	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetSlotPositionAndType() override;
private:
	unsigned int _numberOfSlots = 9;
	std::vector<SlotPositionAndType> _slotPositionAndType;
	float _characterRadius = 25.f;

};

class VShapePattern : public FormationPattern {
public:
	VShapePattern();
	~VShapePattern() {}

	void CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	AnchorPoint GetDriftOffset(std::vector<SlotAssignment> slotAssignments) override;
	AnchorPoint GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) override;

	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetSlotPositionAndType() override;

private:
	unsigned int _numberOfSlots = 9;
	std::vector<SlotPositionAndType> _slotPositionAndType;
	float _offset = 15.f;

};

class FormationManager {
public:
	FormationManager(FormationType formationType, unsigned int maxAmountSlots, AnchorPoint anchorPoint);
	~FormationManager() {}

	bool AddCharacter(std::shared_ptr<EnemyBase> enemyCharacter);
	void UpdateSlots();
	void ReconstructSlotAssignments();
	void RemoveCharacter(std::shared_ptr<EnemyBase> enemyCharacter);

	std::vector<SlotAssignment> GetSlotAssignments();

private:
	AnchorPoint _anchorPoint;
	AnchorPoint _driftOffset;
	std::shared_ptr<FormationPattern> _formationPattern;
	std::vector<SlotAssignment> _slotAssignments;
	float _costLimit = 1500.f;
	float _orientation = 0.f;
	unsigned int _numberOfSlots = 0;
};

