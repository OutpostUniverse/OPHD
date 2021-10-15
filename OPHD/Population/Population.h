#pragma once

#include "Morale.h"
#include "PopulationTable.h"

#include <vector>


class Population
{
public:
	int birthCount() const { return mBirthCount; }
	int deathCount() const { return mDeathCount; }

	void clear();

	const PopulationTable& getPopulations() const;

	void addPopulation(const PopulationTable& population);

	int update(int morale, int food, int residences, int universities, int nurseries, int hospitals);

	void starveRate(float rate) { mStarveRate = rate; }

private:
	int spawnRole(PopulationTable::Role role, int growth, int divisor);
	void spawnPopulation(int morale, int residences, int nurseries, int universities);

	void killRole(PopulationTable::Role role, int divisor);
	void killPopulation(int morale, int nurseries, int hospitals);

	int consumeFood(int food);


	using MoraleModifiers = std::array<MoraleModifier, 5>;


	int mBirthCount{0};
	int mDeathCount{0};

	float mStarveRate{0.5f}; /**< Fraction of population that dies during food shortages. */
	std::size_t mStarveRoleIndex{0};

	PopulationTable mPopulation{}; /**< Current population. */
	PopulationTable mPopulationGrowth{}; /**< Population growth table. */
	PopulationTable mPopulationDeath{}; /**< Population death table. */
};
