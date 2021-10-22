#pragma once

class Population;


struct PopulationRequirements
{
	int workers;
	int scientists;
};


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

	int size() const;

private:
	int mScientistsAsWorkers{0};
	int mScientistsUsed{0};
	int mWorkersUsed{0};

	Population* mPopulation{nullptr};
};
