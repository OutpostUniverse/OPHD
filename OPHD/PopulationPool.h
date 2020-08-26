#pragma once

#include "Population/Population.h"

class PopulationPool
{
public:
	PopulationPool();

	void population(Population* pop);

	int populationAvailable(Population::PersonRole role);
	bool enoughPopulationAvailable(Population::PersonRole role, int amount);
	bool usePopulation(Population::PersonRole role, int amount);

	void clear();

	int scientistsAsWorkers();
	int scientistsEmployed();
	int workersEmployed();
	int populationEmployed();

private:
	int mScientistsAsWorkers{0};
	int mScientistsUsed{0};
	int mWorkersUsed{0};

	Population* mPopulation{nullptr};
};
