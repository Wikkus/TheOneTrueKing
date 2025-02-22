#include "searchSortAlgorithms.h"
#include "formationManager.h"

int SearchSortAlgorithms::Partition(std::vector<int>& objectVector, const int& start, const int& end) {
	_pivotInt = objectVector[start];
	_count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i] <= _pivotInt) {
			_count++;
		}
	}

	_pivotIndex = start + _count;
	std::swap(objectVector[_pivotIndex], objectVector[start]);

	_i = start;
	_k = end;

	while (_i < _pivotIndex && _k > _pivotIndex) {
		while (objectVector[_i] <= _pivotInt) {
			_i++;
		}
		while (objectVector[_k] > _pivotInt) {
			_k--;
		}
		if (_i < _pivotIndex && _k > _pivotIndex) {
			std::swap(objectVector[_i++], objectVector[_k--]);
		}
	}
	return _pivotIndex;
}
void SearchSortAlgorithms::QuickSort(std::vector<int>& objectVector, const int& start, const int& end) {
	if (start >= end) {
		return;
	}
	_partition = Partition(objectVector, start, end);

	QuickSort(objectVector, start, _partition - 1);
	QuickSort(objectVector, _partition + 1, end);
}

int SearchSortAlgorithms::Partition(std::vector<float>& objectVector, const int& start, const int& end) {
	_pivotFloat = objectVector[start];
	_count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i] <= _pivotFloat) {
			_count++;
		}
	}

	_pivotIndex = start + _count;
	std::swap(objectVector[_pivotIndex], objectVector[start]);

	_i = start;
	_k = end;

	while (_i < _pivotIndex && _k > _pivotIndex) {

		while (objectVector[_i] <= _pivotFloat) {
			_i++;
		}

		while (objectVector[_k] > _pivotFloat) {
			_k--;
		}

		if (_i < _pivotIndex && _k > _pivotIndex) {
			std::swap(objectVector[_i++], objectVector[_k--]);
		}
	}
	return _pivotIndex;
}

void SearchSortAlgorithms::QuickSort(std::vector<float>& objectVector, const int& start, const int& end) {
	if (start >= end) {
		return;
	}
	_partition = Partition(objectVector, start, end);

	QuickSort(objectVector, start, _partition - 1);
	QuickSort(objectVector, _partition + 1, end);
}


int SearchSortAlgorithms::Partition(std::vector<CostAndSlot>& objectVector, const int& start, const int& end) {
	_pivotFloat = objectVector[start].cost;

	_count = 0;
	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i].cost <= _pivotFloat) {
			_count++;
		}
	}

	_pivotIndex = start + _count;
	std::swap(objectVector[_pivotIndex], objectVector[start]);

	_i = start;
	_k = end;

	while (_i < _pivotIndex && _k > _pivotIndex) {

		while (objectVector[_i].cost <= _pivotFloat) {
			_i++;
		}

		while (objectVector[_k].cost > _pivotFloat) {
			_k--;
		}

		if (_i < _pivotIndex && _k > _pivotIndex) {
			std::swap(objectVector[_i++], objectVector[_k--]);
		}
	}
	return _pivotIndex;
}
void SearchSortAlgorithms::QuickSort(std::vector<CostAndSlot>& objectVector, const int& start, const int& end) {
	if (start >= end) {
		return;
	}
	_partition = Partition(objectVector, start, end);

	QuickSort(objectVector, start, _partition - 1);
	QuickSort(objectVector, _partition + 1, end);
}


int SearchSortAlgorithms::Partition(std::vector<CharacterAndSlots>& objectVector, const int& start, const int& end) {
	_pivotFloat = objectVector[start].assignmentEase;
	_count = 0;

	for (int i = start + 1; i <= end; i++) {
		if (objectVector[i].assignmentEase <= _pivotFloat) {
			_count++;
		}
	}
	_pivotIndex = start + _count;
	std::swap(objectVector[_pivotIndex], objectVector[start]);

	_i = start;
	_k = end;

	while (_i < _pivotIndex && _k > _pivotIndex) {

		while (objectVector[_i].assignmentEase <= _pivotFloat) {
			_i++;
		}

		while (objectVector[_k].assignmentEase > _pivotFloat) {
			_k--;
		}

		if (_i < _pivotIndex && _k > _pivotIndex) {
			std::swap(objectVector[_i++], objectVector[_k--]);
		}
	}
	return _pivotIndex;
}

void SearchSortAlgorithms::QuickSort(std::vector<CharacterAndSlots>& objectVector, const int& start, const int& end) {
	if (start >= end) {
		return;
	}
	_partition = Partition(objectVector, start, end);

	QuickSort(objectVector, start, _partition - 1);
	QuickSort(objectVector, _partition + 1, end);
}

int SearchSortAlgorithms::BinarySearch(const std::vector<int>& objectIDs, const int& low, const int& high, const int& targetID) {
	_high = high;
	_low = low;	

	while (_low <= _high) {
		_mid = _low + (_high - _low) / 2;
		if (objectIDs[_mid] == targetID) {
			return _mid;
		}
		if (objectIDs[_mid] < targetID) {
			_low = _mid + 1;
		} else {
			_high = _mid - 1;
		}
	}
	return -1;
}
