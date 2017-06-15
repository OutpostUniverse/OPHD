#pragma once

#include "Population/Population.h"

class PopulationPool
{
public:
	PopulationPool();

	~PopulationPool();

	void population(Population* pop);

	int populationAvailable(Population::PersonRole _role);
	bool enoughPopulationAvailable(Population::PersonRole _role, int _amount);
	void usePopulation(Population::PersonRole _role, int _amount);

	void clear();

	int scientistsEmployed();
	int workersEmployed();
	int populationEmployed();

private:

	int				mScientistsUsed;
	int				mWorkersUsed;

	Population*		mPopulation;
};
