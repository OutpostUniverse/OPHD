#pragma once

#include "Population/Population.h"


/**
 * Contains population requirements for a given Structure.
 * 
 * Index 0: Workers.
 * Index 1: Scientists.
 * 
 * Any other index will result in exceptions being thrown.
 */
using PopulationRequirements = std::array<int, 2>;


class PopulationPool
{
public:
	void population(Population* pop);

	int availableWorkers();
	int availableScientists();

	bool usePopulation(PopulationRequirements populationRequirements);

	void clear();

	int scientistsAsWorkers();
	int scientistsEmployed();
	int workersEmployed();
	int populationEmployed();

	int size() const { return mPopulation->size(); }

private:
	bool usePopulation(PopulationTable::Role role, int amount);

	int mScientistsAsWorkers{0};
	int mScientistsUsed{0};
	int mWorkersUsed{0};

	Population* mPopulation{nullptr};
};
