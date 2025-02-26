#include "formationHandler.h"
#include "universalFunctions.h"
#include "debugDrawer.h"
#include "enemyBase.h"
#include "gameEngine.h"
#include "playerCharacter.h"
#include "searchSortAlgorithms.h"

FormationHandler::FormationHandler(const FormationType& formationType, const std::array<unsigned int, 2>& spawnCountPerRow,
	const std::shared_ptr<AnchorPoint>& anchorPoint, const bool& gotOrientation) {
	switch (formationType) {
	case FormationType::DefensiveCircle:
		_formationPattern = std::make_shared<DefensiveCirclePattern>(true);
		break;
	case FormationType::VShape:
		_formationPattern = std::make_shared<VShapePattern>(false);
		break;
	case FormationType::Count:
		break;
	default:
		break;
	}
	_anchorPoint = anchorPoint;
	_gotOrientation = gotOrientation;
	_formationPattern->CreateSlots(spawnCountPerRow, anchorPoint);
}

bool FormationHandler::AddCharacter(std::shared_ptr<EnemyBase> enemyCharacter) {
	//Adds a new character to the first available slot in the formation
	_occupiedSlots = _slotAssignments.size();
	if (_formationPattern->SupportsSlots(_occupiedSlots)) {
		_slotAssignment.enemyCharacter = enemyCharacter;
		_slotAssignment.slotNumber = _slotAssignments.size();
		_numberOfSlots++;
		_slotAssignments.emplace_back(_slotAssignment);
		ReconstructSlotAssignments();
		return true;
	}
	//Returns false if there is no slot available in the formation
	return false;
}

void FormationHandler::UpdateAnchorPoint() {
	_targetObject = playerCharacters.back();

	_anchorPoint->direction = Vector2<float>(_anchorPoint->targetPosition - _anchorPoint->position);
	_anchorPoint->distance = _anchorPoint->direction.absolute();
	_anchorPoint->orientation = universalFunctions->VectorAsOrientation(_targetObject->GetPosition() - _anchorPoint->position);
	if(_inPosition || _slotAssignments.size() <= 0){
		return;

	} else if (_allSlotsOnScreen && _anchorPoint->distance <= 200.f) {
		_inPosition = true;
		return;
	}
	_anchorPoint->direction.normalize();
	_anchorPoint->position += _anchorPoint->direction * deltaTime * _anchorPoint->movementSpeed;
}

void FormationHandler::UpdateSlots() {
	_slotsOnScreen = 0;
	UpdateAnchorPoint();
	//Offsets each character based on the anchor point and current slot location
	for (unsigned int i = 0; i < _slotAssignments.size(); ++i) {
		_slotNumber = _slotAssignments[i].slotNumber;
		_slot = _formationPattern->GetSlotLocation(*_anchorPoint, _slotNumber, _numberOfSlots);

		if (_gotOrientation) {
			_location.position = _anchorPoint->position + 
				_slot.position.rotated(_anchorPoint->orientation - _slot.orientation);
			_location.position -= _driftOffset.position.rotated(_anchorPoint->orientation - _slot.orientation);	

		} else {
			_location.position = _anchorPoint->position + _slot.position;
			_location.position -= _driftOffset.position;
		}
		_location.orientation = _anchorPoint->orientation + _slot.orientation;	
		_location.orientation -= _driftOffset.orientation;

		_slotAssignments[i].enemyCharacter->SetTargetPosition(_location.position);
		_slotAssignments[i].enemyCharacter->SetTargetOrientation(_location.orientation);
		if (!_slotAssignments[i].isOnScreen) {
			if (!universalFunctions->OutsideBorderX(_location.position.x, 25.f) && !universalFunctions->OutsideBorderY(_location.position.y, 25.f)) {
				_slotAssignments[i].isOnScreen = true;
			}
		}
		if (_slotAssignments[i].isOnScreen) {
			_slotsOnScreen++;
		}
	}
	if (_slotsOnScreen >= _slotAssignments.size() - 1) {
		_allSlotsOnScreen = true;
	} else {
		_allSlotsOnScreen = false;
	}
}

void FormationHandler::ReconstructSlotAssignments() {
	_characterAndSlots.clear();
	for (unsigned int i = 0; i < _slotAssignments.size(); ++i) {
		_datum.assignmentEase = 0.f;
		_datum.costAndSlots.clear();
		_datum.enemyCharacter = _slotAssignments[i].enemyCharacter;
		_datum.isAssigned = false;

		//Checks which slots are valid based on the slots class type
		for (unsigned int k = 0; k < _formationPattern->GetNumberOfSlots(); ++k) {
			_slotCost = _formationPattern->GetSlotCost(
				_slotAssignments[i].enemyCharacter->GetWeaponComponent()->GetWeaponType(), k);
			if (_slotCost > _costLimit) {
				continue;
			}
			_slotDatum.slotNumber = k;
			_slotDatum.cost = _slotCost;
			_datum.costAndSlots.emplace_back(_slotDatum);
			//Saves how difficult it would be for the character to be assigned to this slot based on the cost
			_datum.assignmentEase += 1 / (1 + _slotCost);
		}
		//Sorts based on the cost using quicksort
		_datum.costAndSlots = SortByCost(_datum.costAndSlots);
		_characterAndSlots.emplace_back(_datum);
	}
	//Sorts based on assignment ease using quicksort
	_characterAndSlots = SortByAssignmentEase(_characterAndSlots);
	_filledSlots.clear();
	for (unsigned int i = 0; i < _formationPattern->GetNumberOfSlots(); ++i) {
		_filledSlots.emplace_back(false);
	}	
	_slotAssignments.clear();
	//Loop through the characters, the cost for each slot and assign a character to the best suited slot
	for (unsigned int i = 0; i < _characterAndSlots.size(); ++i) {
		for (unsigned int k = 0; k < _characterAndSlots[i].costAndSlots.size(); ++k) {
			//If the slot is not filled, assign a character to that slot
			if (!_filledSlots[_characterAndSlots[i].costAndSlots[k].slotNumber]) {
				_slotAssignment.enemyCharacter = _characterAndSlots[i].enemyCharacter;
 				_slotAssignment.slotNumber = _characterAndSlots[i].costAndSlots[k].slotNumber;
				_filledSlots[_characterAndSlots[i].costAndSlots[k].slotNumber] = true;
				_characterAndSlots[i].isAssigned = true;
				_slotAssignments.emplace_back(_slotAssignment);
				break;
			}
		}
	}
	//Update the driftoffset to match the current formation
	_driftOffset = _formationPattern->GetDriftOffset(*_anchorPoint, _slotAssignments);
}

void FormationHandler::RemoveCharacter(std::shared_ptr<EnemyBase> enemyCharacter) {
	for (unsigned int i = 0; i < _slotAssignments.size(); ++i) {
		if (_slotAssignments[i].enemyCharacter->GetObjectID() == enemyCharacter->GetObjectID()) {
			_slotAssignments[i] = _slotAssignments.back();	
			_slotAssignments.pop_back();
			_numberOfSlots--;
			_formationPattern->SetNumberOfSlots(_numberOfSlots);
			break;
		}
	}
	//ReconstructSlotAssignments();
}

std::vector<SlotAssignment> FormationHandler::GetSlotAssignments() {
	return _slotAssignments;
}

std::shared_ptr<FormationPattern> FormationHandler::GetFormationPattern() {
	return _formationPattern;
}

const bool FormationHandler::GetInPosition() const {
	return _inPosition;
}

const unsigned int FormationHandler::GetNumberOfSlots() const {
	return _numberOfSlots;
}

DefensiveCirclePattern::DefensiveCirclePattern(const bool& lockOrientation) : FormationPattern(lockOrientation) {}

void DefensiveCirclePattern::CreateSlots(const std::array<unsigned int, 2>& spawnCountPerRow, const std::shared_ptr<AnchorPoint>& anchorPoint) {
	_numberOfSlots = spawnCountPerRow[0] * spawnCountPerRow[1];

	//When there is only one layer, every other slot roles created are mages and the others are swordsmen
	if (spawnCountPerRow[1] == 1) {
		CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _characterRadius,
			SlotAttackType::Count, false);

	} else if (spawnCountPerRow[1] % 2 == 0) {
		//If there are an even amout of layers, the outer slot roles are swordsmen and the inner are mages
		_amountSlots = std::round(spawnCountPerRow[0] * 1.25f);
		for (unsigned int i = 0; i < spawnCountPerRow[1]; ++i) {
			if (i < std::round(spawnCountPerRow[1] * 0.5f)) {
				CreateSlotsOfType(anchorPoint, _amountSlots, _characterRadius,
					SlotAttackType::Defender, true);
			} else {
				CreateSlotsOfType(anchorPoint, (spawnCountPerRow[0] * 2) - _amountSlots, _characterRadius,
					SlotAttackType::Mage, false);
			}
			//When a layer has been created, I subtract a characterRadius from the distanceBetweenSlots,
			// which creates the next layer of slots in a smaller circle
		}
	} else {
		//If there are an odd amount of layers(at least 3) the outer layers are defenders, 
		// middle are swordsmen and inner are mages
		for (unsigned int i = 0; i < spawnCountPerRow[1]; ++i) {
			if (i < std::round(spawnCountPerRow[1] * 0.33f)) {
				CreateSlotsOfType(anchorPoint, std::round(spawnCountPerRow[0] * 1.5f), _characterRadius,
					SlotAttackType::Defender, true);

			} else if (i < std::round(spawnCountPerRow[1] * 0.66f)) {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _characterRadius,
					SlotAttackType::Swordsman, true);

			} else {
				CreateSlotsOfType(anchorPoint, std::round(spawnCountPerRow[0] * 0.5f), _characterRadius,
					SlotAttackType::Mage, false);
			}
		}
	}
}

void DefensiveCirclePattern::CreateSlotsOfType(const std::shared_ptr<AnchorPoint>& anchorPoint, const unsigned int& amountSlots, const float& radius,
	const SlotAttackType& attackType, const bool& lockOrientation) {
	for (unsigned int i = 0; i < amountSlots; ++i) {
		//I'm setting the slots orientation based on the current slot number compared to the amount of slots with the help of PI * 2,
		//  which makes the slots orientation point away from the anchorPoint from where it will be positioned
		_angleAroundCircle = (i / (float)amountSlots * PI * 2);
		_radius = radius / sin(PI / (float)amountSlots);

		//Using sin and cos to set the position of the slot based on the orientation
		_result.position.x = _radius * sin(_angleAroundCircle);
		_result.position.y = _radius * -cos(_angleAroundCircle);

		//If the orientation is locked, then I keep the angleAroundCircle as it is currently
		if (!lockOrientation) {
			//If the orientation is not locked, I set the angleAroundCircle based on the slots position compared to the anchorPoints
			_angleAroundCircle = universalFunctions->VectorAsOrientation(anchorPoint->position - _result.position) - PI * 0.5f;
		}
		_result.orientation = universalFunctions->WrapMinMax(_angleAroundCircle, -PI, PI);
		if (attackType == SlotAttackType::Count) {
			if (i % 2 == 0) {
				_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
					_result.orientation, _result.position, SlotAttackType::Swordsman));
				_amountSlotsPerType[SlotAttackType::Swordsman] += 1;
			} else {
				_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
					_result.orientation, _result.position, SlotAttackType::Mage));
				_amountSlotsPerType[SlotAttackType::Mage] += 1;
			}

		} else {
			_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
				_result.orientation, _result.position, attackType));
			_amountSlotsPerType[attackType] += 1;
		}
	}
}

std::vector<CostAndSlot> SortByCost(std::vector<CostAndSlot> slotsAndCosts) {
	searchSort->QuickSort(slotsAndCosts, 0, slotsAndCosts.size() - 1);
	return slotsAndCosts;
}

std::vector<CharacterAndSlots> SortByAssignmentEase(std::vector<CharacterAndSlots> characterAndSlots) {
	searchSort->QuickSort(characterAndSlots, 0, characterAndSlots.size() - 1);
	return characterAndSlots;
}

VShapePattern::VShapePattern(const bool& lockOrientation) : FormationPattern(lockOrientation) {
	_lockOrientation = lockOrientation;
}

void VShapePattern::CreateSlots(const std::array<unsigned int, 2>& spawnCountPerRow, const std::shared_ptr<AnchorPoint>& anchorPoint) {
	_numberOfSlots = spawnCountPerRow[0] * spawnCountPerRow[1];

	//Creates the slots with a role based on the amounts of rows the circle will have
	//At one row, every other slot role will be for a swordsman and mages
	if (spawnCountPerRow[1] == 1) {
		CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Count, _lockOrientation);

	} else if (spawnCountPerRow[1] % 2 == 0) {
		//If there are an even amount of rows, the slot roles in rows further out will be defenders, 
		// the inner rows roles will be meages
		for (unsigned int i = 0; i < spawnCountPerRow[1]; ++i) {
			if (i < spawnCountPerRow[1] / 2) {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Defender, _lockOrientation);
				_frontPosition.y += _rowDistance;

			} else {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Mage, _lockOrientation);
				_frontPosition.y += _rowDistance;
			}
		}
	} else {
		//Otherwise all three roles will be created with defenders at the outer rows, 
		// swordsmen in the middle row and mages in the inner row
		for (unsigned int i = 0; i < spawnCountPerRow[1]; ++i) {
			if (i < std::round(spawnCountPerRow[1] * 0.33f)) {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Defender, _lockOrientation);
				_frontPosition.y += _rowDistance;

			} else if (i < std::round(spawnCountPerRow[1] * 0.66f)) {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Swordsman, _lockOrientation);
				_frontPosition.y += _rowDistance;

			} else {
				CreateSlotsOfType(anchorPoint, spawnCountPerRow[0], _frontPosition, SlotAttackType::Mage, _lockOrientation);
				_frontPosition.y += _rowDistance;
			}
		}
	}
	_frontPosition.y = 0.f;
}

void VShapePattern::CreateSlotsOfType(const std::shared_ptr<AnchorPoint>& anchorPoint, const unsigned int& amountSlots,
	const Vector2<float>& frontSlotPosition, const SlotAttackType& attackType, const bool& lockOrientation) {
	//Creates a V-shaped row based on the offset multiplied by the current amount of slots 
	// and adds it to the vector of slots
	for (unsigned int i = 0; i < amountSlots; ++i) {
		if (i % 2 == 0) {
			_positionOffset = _slotOffsetA * _multiplier;
			_multiplier++;
		} else {
			_positionOffset = _slotOffsetB * _multiplier;
		}

		_currentPosition = frontSlotPosition + _positionOffset;
		_currentPosition = (anchorPoint->position + _currentPosition) - anchorPoint->position;
		_currentPosition.rotate(anchorPoint->orientation);

		if (attackType == SlotAttackType::Count) {
			if (_multiplier % 2 == 0) {
				_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
					anchorPoint->orientation, _currentPosition, SlotAttackType::Mage));
				_amountSlotsPerType[SlotAttackType::Mage]++;

			} else {
				_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
					anchorPoint->orientation, _currentPosition, SlotAttackType::Swordsman));
				_amountSlotsPerType[SlotAttackType::Swordsman]++;
			}
		} else {
			_slotPositionAndType.emplace_back(SlotPositionAndType(_slotPositionAndType.size(), lockOrientation,
				anchorPoint->orientation, _currentPosition, attackType));
			_amountSlotsPerType[attackType]++;
		}
	}
	_multiplier = 0;
}

FormationPattern::FormationPattern(const bool& lockOrientation) {
	_lockOrientation = lockOrientation;
}

StaticCharacter FormationPattern::GetDriftOffset(const AnchorPoint& anchorPoint, const std::vector<SlotAssignment>& slotAssignments) {
	_result.position = { 0.f, 0.f };
	_result.orientation = 0.f;

	//Calculates the drift offset of the slots which will make them evenly centered around the anchor point
	for (unsigned int i = 0; i < slotAssignments.size(); ++i) {
		_location = GetSlotLocation(anchorPoint, slotAssignments[i].slotNumber, slotAssignments.size());
		_result.position += _location.position;
		_result.orientation += _location.orientation;
	}
	_result.position /= slotAssignments.size();
	_result.orientation /= slotAssignments.size();
	return _result;
}

StaticCharacter FormationPattern::GetSlotLocation(const AnchorPoint& anchorPoint, const unsigned int& slotNumber, const unsigned int& numberOfSlots) {
	_currentLocation.position = _slotPositionAndType[slotNumber].position;
	_currentLocation.orientation = _slotPositionAndType[slotNumber].orientation;
	if (_slotPositionAndType[slotNumber].lockOrientation) {
		_currentLocation.orientation -= anchorPoint.orientation;
	}
	_currentLocation.orientation = universalFunctions->WrapMinMax(_currentLocation.orientation, -PI, PI);
	return _currentLocation;
}

std::unordered_map<SlotAttackType, unsigned int> FormationPattern::GetSlotsPerType() const {
	return _amountSlotsPerType;
}

const float FormationPattern::GetSlotCost(const WeaponType& weaponType, const unsigned int& index) const {
	switch (weaponType) {
	case WeaponType::Shield:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Defender:
			return 0.f;
		case SlotAttackType::Mage:
			return 2000.f;
		case SlotAttackType::Swordsman:
			return 500.f;
		default:
			break;
		}
		break;
	case WeaponType::Staff:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Defender:
			return 2000.f;
		case SlotAttackType::Mage:
			return 0.f;
		case SlotAttackType::Swordsman:
			return 1000.f;
		default:
			break;
		}
		break;
	case WeaponType::Sword:
		switch (_slotPositionAndType[index].slotAttackType) {
		case SlotAttackType::Defender:
			return 500.f;
		case SlotAttackType::Mage:
			return 1500.f;
		case SlotAttackType::Swordsman:
			return 0.f;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

const unsigned int FormationPattern::GetNumberOfSlots() const {
	return _numberOfSlots;
}

bool FormationPattern::SupportsSlots(const unsigned int& slotCount) {
	if (slotCount <= _numberOfSlots) {
		return true;
	}
	return false;
}

void FormationPattern::SetNumberOfSlots(const unsigned int& numberOfSlots) {
	_numberOfSlots = numberOfSlots;
}
