#include "formationManager.h"
#include "debugDrawer.h"
#include "enemyBase.h"
#include "enemyHuman.h"
#include "gameEngine.h"
#include "playerCharacter.h"

FormationManager::FormationManager(FormationType formationType, unsigned int maxAmountSlots, AnchorPoint anchorPoint) {
	switch (formationType) {
	case FormationType::DefensiveCircle:
		_formationPattern = std::make_shared<DefensiveCirclePattern>();
		break;
	case FormationType::SlotRole:
		_formationPattern = std::make_shared<SlotRolePattern>();
		break;	
	case FormationType::VShape:
		_formationPattern = std::make_shared<VShapePattern>();
		break;
	case FormationType::Count:
		break;
	default:
		break;
	}
	_anchorPoint = anchorPoint;
	_formationPattern->CreateSlots(maxAmountSlots, anchorPoint);
}

bool FormationManager::AddCharacter(std::shared_ptr<EnemyBase> enemyCharacter) {
	//Adds a new character to the first available slot in the formation
	unsigned int occupiedSlots = _slotAssignments.size();
	if (_formationPattern->SupportsSlots(occupiedSlots)) {
		SlotAssignment slotAssignment;
		slotAssignment.enemyCharacter = enemyCharacter;
		slotAssignment.slotNumber = _slotAssignments.size();
		_numberOfSlots++;
		_slotAssignments.emplace_back(slotAssignment);
		ReconstructSlotAssignments();
		return true;
	}
	//Returns false if there is no slot available in the formation
	return false;
}

void FormationManager::UpdateSlots() {
	Vector2<float> direction = Vector2<float>(playerCharacter->GetPosition() - _anchorPoint.position).normalized();
	_anchorPoint.position += direction * deltaTime * 50.f;
	_anchorPoint.orientation = VectorAsOrientation(direction);
	debugDrawer->AddDebugCross(_anchorPoint.position, 25.f, { 75, 255, 175, 255 });

	//Offsets each character based on the anchor point and current slot location
	for (unsigned int i = 0; i < _slotAssignments.size(); i++) {
		unsigned int slotNumber = _slotAssignments[i].slotNumber;
		AnchorPoint slot = _formationPattern->GetSlotLocation(slotNumber, _numberOfSlots);
		AnchorPoint location;
		location.position = _anchorPoint.position + slot.position;
		location.orientation = _anchorPoint.orientation + slot.orientation;

		location.position -= _driftOffset.position;
		location.orientation -= _driftOffset.orientation;
		_slotAssignments[i].enemyCharacter->SetTargetPosition(location.position);
		_slotAssignments[i].enemyCharacter->SetTargetOrientation(location.orientation);

		debugDrawer->AddDebugCross(location.position, 25.f, { 75, 255, 175, 255 });
	}
}

void FormationManager::ReconstructSlotAssignments() {
	std::vector<CharacterAndSlots> characterAndSlots;
	for (unsigned int i = 0; i < _slotAssignments.size(); ++i) {
		//Checks which slots are valid based on the slots class type
		CharacterAndSlots datum;
		datum.enemyCharacter = _slotAssignments[i].enemyCharacter;	
		for (unsigned int k = 0; k < _formationPattern->GetNumberOfSlots(); ++k) {
			float slotCost = _formationPattern->GetSlotCost(
				_slotAssignments[i].enemyCharacter->GetWeaponComponent()->GetWeaponType(), k);
			if (slotCost >= _costLimit) {
				continue;
			}
			CostAndSlot slotDatum;
			slotDatum.slotNumber = k;
			slotDatum.cost = slotCost;
			datum.costAndSlots.emplace_back(slotDatum);
			//Saves how difficult it would be for the character to be assigned to this slot based on the cost
			datum.assignmentEase += 1 / (1 + slotCost);
		}
		//Sorts based on the cost using quicksort
		datum.costAndSlots = SortByCost(datum.costAndSlots);
		characterAndSlots.emplace_back(datum);
	}
	//Sorts based on assignment ease using quicksort
	characterAndSlots = SortByAssignmentEase(characterAndSlots);
	
	//Create a vector of bools which will keep track of which slot is available
	std::vector<bool> filledSlots;
	for (unsigned int i = 0; i < _formationPattern->GetNumberOfSlots(); i++) {
		filledSlots.emplace_back(false);
	}
	//Loop through the characters, the cost for each slot and assign a character to the best suited slot
	std::vector<SlotAssignment> slotAssignments;
	std::vector<CharacterAndSlots> unassignedSlots;
	for (unsigned int i = 0; i < characterAndSlots.size(); i++) {
		for (unsigned int k = 0; k < characterAndSlots[i].costAndSlots.size(); k++) {
			//If the slot is not filled, assign a character to that slot
			if (!filledSlots[characterAndSlots[i].costAndSlots[k].slotNumber]) {
				SlotAssignment assignment;
				assignment.enemyCharacter = characterAndSlots[i].enemyCharacter;
 				assignment.slotNumber = characterAndSlots[i].costAndSlots[k].slotNumber;
				filledSlots[characterAndSlots[i].costAndSlots[k].slotNumber] = true;
				characterAndSlots[i].isAssigned = true;
				slotAssignments.emplace_back(assignment);
				break;
			}
		}
		if (!characterAndSlots[i].isAssigned) {
			unassignedSlots.emplace_back(characterAndSlots[i]);
		}
	}
	//See if any of the slot assignments has not been assigned
	while (unassignedSlots.size() > 0) {
		if (filledSlots.size() < 1) {
			break;
		}
		for (unsigned int i = 0; i < filledSlots.size(); ++i) {
			//If a slot is not assigned, assign it to the first available slot
			if (!filledSlots[i]) {
				SlotAssignment assignment;
				assignment.enemyCharacter = unassignedSlots.back().enemyCharacter;
				assignment.slotNumber = i;
				slotAssignments.emplace_back(assignment);
				unassignedSlots.pop_back();
				filledSlots[i] = true;
				break;
			}
		}
	}
	//Overwrite the current slot assignments with the updated ones
	_slotAssignments = slotAssignments;
	_driftOffset = _formationPattern->GetDriftOffset(_slotAssignments);
}

void FormationManager::RemoveCharacter(std::shared_ptr<EnemyBase> enemyCharacter) {
	SlotAssignment slotAssignment;
	for (unsigned int i = 0; i < _slotAssignments.size(); i++) {
		if (_slotAssignments[i].enemyCharacter->GetObjectID() == enemyCharacter->GetObjectID()) {
			_slotAssignments[i] = _slotAssignments.back();
			_slotAssignments.pop_back();
			break;
		}
	}
	ReconstructSlotAssignments();
}

std::vector<SlotAssignment> FormationManager::GetSlotAssignments() {
	return _slotAssignments;
}

void DefensiveCirclePattern::CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) {}

unsigned int DefensiveCirclePattern::CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) {
	unsigned int filledSlots = 0;
	for (unsigned int i = 0; i < slotAssignments.size(); i++) {
		filledSlots = slotAssignments[i].slotNumber;
	}
	return filledSlots + 1;
}

AnchorPoint DefensiveCirclePattern::GetDriftOffset(std::vector<SlotAssignment> slotAssignments) {
	AnchorPoint result;
	for (unsigned int i = 0; i < slotAssignments.size(); i++) {
		AnchorPoint location = GetSlotLocation(slotAssignments[i].slotNumber, slotAssignments.size());
		result.position += location.position;
		result.orientation += location.orientation;
	}
	result.position /= slotAssignments.size();
	result.orientation /= slotAssignments.size();
	return result;
}

AnchorPoint DefensiveCirclePattern::GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) {
	float angleAroundCircle = slotNumber / numberOfSlots * PI * 2;
	float radius = _characterRadius / sin(PI / numberOfSlots);

	AnchorPoint result;
	result.position.x = radius * sin(angleAroundCircle);
	result.position.y = radius * -cos(angleAroundCircle);
	result.orientation = WrapMinMax(angleAroundCircle, -PI, PI);
	return result;
}

float DefensiveCirclePattern::GetSlotCost(WeaponType weaponType, unsigned int index) {
	return 0.0f;
}

const unsigned int DefensiveCirclePattern::GetNumberOfSlots() const {
	return _numberOfSlots;
}

bool DefensiveCirclePattern::SupportsSlots(unsigned int slotCount) {
	return true;
}

void DefensiveCirclePattern::SetSlotPositionAndType() {
	
}

SlotRolePattern::SlotRolePattern() {

}

void SlotRolePattern::CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) {
	_slotPositionAndType.emplace_back(SlotPositionAndType(0, AnchorPoint(anchorPoint.borderSide, Vector2(-50.f, 0.f), 0), SlotAttackType::Melee));
	_slotPositionAndType.emplace_back(SlotPositionAndType(1, AnchorPoint(anchorPoint.borderSide, Vector2(-25.f, -25.f), 0), SlotAttackType::Magic));
	_slotPositionAndType.emplace_back(SlotPositionAndType(2, AnchorPoint(anchorPoint.borderSide, Vector2(-25.f, 25.f), 0), SlotAttackType::Magic));
	_slotPositionAndType.emplace_back(SlotPositionAndType(3, AnchorPoint(anchorPoint.borderSide, Vector2(0.f, 50.f), 0), SlotAttackType::Melee));
	_slotPositionAndType.emplace_back(SlotPositionAndType(4, AnchorPoint(anchorPoint.borderSide, Vector2(0.f, 0.f), 0), SlotAttackType::Magic));
	_slotPositionAndType.emplace_back(SlotPositionAndType(5, AnchorPoint(anchorPoint.borderSide, Vector2(0.f, -50.f), 0), SlotAttackType::Melee));
	_slotPositionAndType.emplace_back(SlotPositionAndType(6, AnchorPoint(anchorPoint.borderSide, Vector2(25.f, 25.f), 0), SlotAttackType::Magic));
	_slotPositionAndType.emplace_back(SlotPositionAndType(7, AnchorPoint(anchorPoint.borderSide, Vector2(25.f, -25.f), 0), SlotAttackType::Magic));
	_slotPositionAndType.emplace_back(SlotPositionAndType(8, AnchorPoint(anchorPoint.borderSide, Vector2(50.f, 0.f), 0), SlotAttackType::Melee));
}

unsigned int SlotRolePattern::CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) {
	return slotAssignments.size();
}

AnchorPoint SlotRolePattern::GetDriftOffset(std::vector<SlotAssignment> slotAssignments) {
	AnchorPoint result;
	for (unsigned int i = 0; i < slotAssignments.size(); i++) {
		AnchorPoint location = GetSlotLocation(slotAssignments[i].slotNumber, slotAssignments.size());
		result.position += location.position;
		result.orientation += location.orientation;
	}
	result.position /= slotAssignments.size();
	result.orientation /= slotAssignments.size();
	return result;
}

AnchorPoint SlotRolePattern::GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) {
	AnchorPoint result;
	result.position = _slotPositionAndType[slotNumber].positionAndOrientation.position;
	result.orientation = _slotPositionAndType[slotNumber].positionAndOrientation.orientation;
	result.orientation = WrapMinMax(result.orientation, -PI, PI);
	return result;
}

float SlotRolePattern::GetSlotCost(WeaponType weaponType, unsigned int index) {	
	switch (weaponType) {
	//case WeaponType::Bow:
	//	switch (_slotPositionAndType[index].slotAttackType) {
	//	case SlotAttackType::Magic:
	//		return 1000.f;
	//	case SlotAttackType::Melee:
	//		return 1500.f;
	//	case SlotAttackType::Missle:
	//		return 0.f;
	//	default:
	//		break;
	//	}
	//	break;
	case WeaponType::Staff:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Magic:
			return 0.f;
		case SlotAttackType::Melee:
			return 1500.f;
		//case SlotAttackType::Missle:
		//	return 500.f;
		default:
			break;
		}
		break;
	case WeaponType::Sword:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Magic:
			return 1500.f;
		case SlotAttackType::Melee:
			return 0.f;
		//case SlotAttackType::Missle:
		//	return 1500.f;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

const unsigned int SlotRolePattern::GetNumberOfSlots() const {
	return _numberOfSlots;
}

bool SlotRolePattern::SupportsSlots(unsigned int slotCount) {
	if (slotCount <= _numberOfSlots) {
		return true;
	}
	return false;
}

void SlotRolePattern::SetSlotPositionAndType() {
}

std::vector<CostAndSlot> SortByCost(std::vector<CostAndSlot> slotsAndCosts) {
	QuickSort(slotsAndCosts, 0, slotsAndCosts.size() - 1);
	return slotsAndCosts;
}

std::vector<CharacterAndSlots> SortByAssignmentEase(std::vector<CharacterAndSlots> characterAndSlots) {
	QuickSort(characterAndSlots, 0, characterAndSlots.size() - 1);
	return characterAndSlots;
}

VShapePattern::VShapePattern() {}

void VShapePattern::CreateSlots(unsigned int slotCount, AnchorPoint anchorPoint) {
	Vector2<float> position1;
	Vector2<float> position2;
	
	switch (anchorPoint.borderSide) {
	case BorderSide::Top:
		position1 = { -_offset, -_offset };
		position2 = { _offset, -_offset };
		break;
	case BorderSide::Left:
		position1 = { -_offset, -_offset };
		position2 = { -_offset, _offset };
		break;
	case BorderSide::Right:
		position1 = { _offset, -_offset };
		position2 = { _offset, _offset };
		break;
	case BorderSide::Bottom:
		position1 = { -_offset, _offset };
		position2 = { _offset, _offset };
		break;
	default:
		break;
	}

	for (unsigned int i = 0; i < slotCount; i++) {
		if (i < slotCount / 2) {
			if (i % 2 == 0) {
				_slotPositionAndType.emplace_back(SlotPositionAndType(
					i, AnchorPoint(anchorPoint.borderSide, { i * position1.x, i * position1.y }, 0), SlotAttackType::Melee));

			} else {
				_slotPositionAndType.emplace_back(SlotPositionAndType(
					i, AnchorPoint(anchorPoint.borderSide, { i * position2.x, i * position2.y }, 0), SlotAttackType::Melee));
			}
		} else {
			if (i % 2 == 0) {
				_slotPositionAndType.emplace_back(SlotPositionAndType(
					i, AnchorPoint(anchorPoint.borderSide, { i * position1.x, i * position1.y }, 0), SlotAttackType::Magic));

			} else {
				_slotPositionAndType.emplace_back(SlotPositionAndType(
					i, AnchorPoint(anchorPoint.borderSide, { i * position2.x, i * position2.y }, 0), SlotAttackType::Magic));
			}
		}
	}
}

unsigned int VShapePattern::CalculateNumberOfSlots(std::vector<SlotAssignment> slotAssignments) {
	return slotAssignments.size();
}

AnchorPoint VShapePattern::GetDriftOffset(std::vector<SlotAssignment> slotAssignments) {
	AnchorPoint result;
	for (unsigned int i = 0; i < slotAssignments.size(); i++) {
		AnchorPoint location = GetSlotLocation(slotAssignments[i].slotNumber, slotAssignments.size());
		result.position += location.position;
		result.orientation += location.orientation;
	}
	result.position /= slotAssignments.size();
	result.orientation /= slotAssignments.size();
	return result;
}

AnchorPoint VShapePattern::GetSlotLocation(unsigned int slotNumber, unsigned int numberOfSlots) {
	AnchorPoint result;
	result.position = _slotPositionAndType[slotNumber].positionAndOrientation.position;
	result.orientation = _slotPositionAndType[slotNumber].positionAndOrientation.orientation;
	result.orientation = WrapMinMax(result.orientation, -PI, PI);
	return result;
}

float VShapePattern::GetSlotCost(WeaponType weaponType, unsigned int index) {
	switch (weaponType) {
		//case WeaponType::Bow:
		//	switch (_slotPositionAndType[index].slotAttackType) {
		//	case SlotAttackType::Magic:
		//		return 1000.f;
		//	case SlotAttackType::Melee:
		//		return 1500.f;
		//	case SlotAttackType::Arrow:
		//		return 0.f;
		//	default:
		//		break;
		//	}
		//	break;
	case WeaponType::Staff:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Magic:
			return 0.f;
		case SlotAttackType::Melee:
			return 2000.f;
			//case SlotAttackType::Arrow:
			//	return 500.f;
		default:
			break;
		}
		break;
	case WeaponType::Sword:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Magic:
			return 2000.f;
		case SlotAttackType::Melee:
			return 0.f;
			//case SlotAttackType::Arrow:
			//	return 1500.f;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

const unsigned int VShapePattern::GetNumberOfSlots() const {
	return _numberOfSlots;
}

bool VShapePattern::SupportsSlots(unsigned int slotCount) {
	if (slotCount <= _numberOfSlots) {
		return true;
	}
	return false;
}

void VShapePattern::SetSlotPositionAndType() {}
