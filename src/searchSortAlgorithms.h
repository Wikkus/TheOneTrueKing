#pragma once
#include "vector2.h"

#include <vector>

struct CostAndSlot;
struct CharacterAndSlots;

class SearchSortAlgorithms {
public:
	SearchSortAlgorithms() {}
	~SearchSortAlgorithms() {}

	int Partition(std::vector<CostAndSlot>& objectVector, const int& start, const int& end);
	void QuickSort(std::vector<CostAndSlot>& objectVector, const int& start, const int& end);

	int Partition(std::vector<CharacterAndSlots>& objectVector, const int& start, const int& end);
	void QuickSort(std::vector<CharacterAndSlots>& objectVector, const int& start, const int& end);

	int BinarySearch(const std::vector<int>& objectIDs, const int& low, const int& high, const int& targetID);

private:
	//Quicksort
	float _pivotFloat = 0.f;
	float _pivotInt = 0;
	int _pivotIndex = 0;
	int _count = 0;
	int _i = 0;
	int _k = 0;
	int _partition = 0;

	//Binary search
	int _high = -1;
	int _low = -1;
	int _mid = -1;
};