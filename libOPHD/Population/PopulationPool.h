#pragma once


struct PopulationRequirements;
class PopulationModel;


class PopulationPool
{
public:
	void population(PopulationModel* pop);

	int availableWorkers() const;
	int availableScientists() const;

	bool usePopulation(PopulationRequirements populationRequirements);

	void clear();

	int scientistsAsWorkers() const;
	int scientistsEmployed() const;
	int workersEmployed() const;
	int populationEmployed() const;

	int size() const;

private:
	int mScientistsAsWorkers{0};
	int mScientistsUsed{0};
	int mWorkersUsed{0};

	PopulationModel* mPopulationModel{nullptr};
};
