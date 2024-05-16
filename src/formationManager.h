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
	VShape,
	Count
};

enum class SlotAttackType {
	Defender,
	MagicAttacker,
	MeleeAttacker,
	Count
};

struct CostAndSlot {
	float cost = 0.f;
	unsigned int slotNumber = 0;
};

struct SlotPositionAndType {
	unsigned int slotNumber = 0;
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

	virtual AnchorPoint GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) = 0;
	virtual AnchorPoint GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) = 0;

	virtual float GetSlotCost(WeaponType weaponType, unsigned int index) = 0;

	virtual const unsigned int GetNumberOfSlots() const = 0;

	virtual bool SupportsSlots(unsigned int slotCount) = 0;
	
	virtual void SetNumberOfSlots(unsigned int numberOfSlots) = 0;

protected:	
	SlotAttackType _attackType = SlotAttackType::Count;
	unsigned int _numberOfSlots = 0;
};

class DefensiveCirclePattern : public FormationPattern {
public:
	DefensiveCirclePattern(bool holdOrientation);
	~DefensiveCirclePattern() {}
	void CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	AnchorPoint GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) override;
	AnchorPoint GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) override;
	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetNumberOfSlots(unsigned int numberOfSlots) override;

private:
	bool _holdOrientation = false;
	float _characterRadius = 20.f;
};

class VShapePattern : public FormationPattern {
public:
	VShapePattern(SlotAttackType attackType);
	~VShapePattern() {}

	void CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) override;

	unsigned int CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) override;

	AnchorPoint GetDriftOffset(AnchorPoint anchorPoint, std::vector<SlotAssignment> slotAssignments) override;
	AnchorPoint GetSlotLocation(AnchorPoint anchorPoint, unsigned int slotNumber, unsigned int numberOfSlots) override;

	float GetSlotCost(WeaponType weaponType, unsigned int index) override;

	const unsigned int GetNumberOfSlots() const override;

	bool SupportsSlots(unsigned int slotCount) override;

	void SetNumberOfSlots(unsigned int numberOfSlots) override;

private:
	void CreateSlotsOfType(AnchorPoint anchorPoint, unsigned int amountSlots, Vector2<float> frontSlotPosition, SlotAttackType attackType);
	
	std::vector<SlotPositionAndType> _slotPositionAndType;
	std::vector<std::vector<SlotPositionAndType>> _slotPositionAndTypeGroup;
	float _offset = 15.f;
	float _multiplier = 0.f;
	
	Vector2<float> _frontPosition = { 0.f, 0.f };
	Vector2<float> _rowDistance = { 0.f, 32.f };

	Vector2<float> _positionOffset = { 0.f, 0.f };

	Vector2<float> _slotOffsetA = { -32.f, 32.f };
	Vector2<float> _slotOffsetB = { 32.f, 32.f };

};

class FormationManager {
public:
	FormationManager(FormationType formationType, unsigned int maxAmountSlots, AnchorPoint anchorPoint, bool holdOrientation, SlotAttackType attackType);
	~FormationManager() {}

	bool AddCharacter(std::shared_ptr<EnemyBase> enemyCharacter);
	void UpdateSlots();
	void ReconstructSlotAssignments();
	void RemoveCharacter(std::shared_ptr<EnemyBase> enemyCharacter);

	std::vector<SlotAssignment> GetSlotAssignments();

	const unsigned int GetNumberOfSlots() const;

private:
	AnchorPoint _anchorPoint;
	AnchorPoint _driftOffset;
	std::shared_ptr<FormationPattern> _formationPattern;
	std::vector<SlotAssignment> _slotAssignments;
	float _costLimit = 1500.f;
	float _orientation = 0.f;
	unsigned int _numberOfSlots = 0;
};

