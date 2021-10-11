#pragma once

#include "Population/Population.h"


class PopulationPool
{
public:
	void population(Population* pop);

	int availableWorkers();
	int availableScientists();

	int populationAvailable(PopulationTable::Role role);
	bool usePopulation(PopulationTable::Role role, int amount);

	void clear();

	int scientistsAsWorkers();
	int scientistsEmployed();
	int workersEmployed();
	int populationEmployed();

	int size() const { return mPopulation->size(); }

private:
	int mScientistsAsWorkers{0};
	int mScientistsUsed{0};
	int mWorkersUsed{0};

	Population* mPopulation{nullptr};
};
